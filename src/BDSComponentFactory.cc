#include "BDSComponentFactory.hh"

// elements
#include "BDSAwakeScintillatorScreen.hh"
#include "BDSCavityRF.hh"
#include "BDSCollimatorElliptical.hh"
#include "BDSCollimatorRectangular.hh"
#include "BDSDegrader.hh"
#include "BDSDrift.hh"
#include "BDSElement.hh"
#include "BDSLaserWire.hh"
#include "BDSLine.hh"
#include "BDSMagnet.hh"
#include "BDSSamplerPlane.hh"
#include "BDSScintillatorScreen.hh"
#include "BDSTerminator.hh"
#include "BDSTeleporter.hh"
#include "BDSTiltOffset.hh"
#include "BDSTransform3D.hh"

// general
#include "BDSAcceleratorComponentRegistry.hh"
#include "BDSBeamPipeType.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSCavityInfo.hh"
#include "BDSCavityType.hh"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldType.hh"
#include "BDSIntegratorSet.hh"
#include "BDSIntegratorType.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetGeometryType.hh"
#include "BDSMagnetStrength.hh"
#include "BDSMagnetType.hh"
#include "BDSParser.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4GeometryTolerance.hh"
#include "G4Transform3D.hh"

#include "parser/elementtype.h"
#include "parser/cavitymodel.h"

#include <cmath>
#include <string>
using namespace GMAD;


BDSComponentFactory::BDSComponentFactory()
{
  lengthSafety = BDSGlobalConstants::Instance()->LengthSafety();
  
  // compute magnetic rigidity brho
  // formula: B(Tesla)*rho(m) = p(GeV)/(0.299792458 * |charge(e)|)
  // charge (in e units)
  charge = BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGCharge();

  // momentum (in GeV/c)
  G4double momentum = BDSGlobalConstants::Instance()->BeamMomentum()/CLHEP::GeV;

  // rigidity (in T*m)
  brho = BDSGlobalConstants::Instance()->FFact()*( momentum / 0.299792458);
  
  // rigidity (in Geant4 units)
  brho *= (CLHEP::tesla*CLHEP::m);

  G4cout << "Rigidity (Brho) : "<< std::abs(brho)/(CLHEP::tesla*CLHEP::m) << " T*m"<<G4endl;

  integratorSet = BDS::IntegratorSet(BDSGlobalConstants::Instance()->IntegratorSet());

  // prepare rf cavity model info from parser
  PrepareCavityModels();
  
  notSplit = BDSGlobalConstants::Instance()->DontSplitSBends();
  includeFringe = BDSGlobalConstants::Instance()->IncludeFringeFields();
  thinElementLength = BDSGlobalConstants::Instance()->ThinElementLength();
}

BDSComponentFactory::~BDSComponentFactory()
{
  for(auto info : cavityInfos)
    {delete info.second;}
}

BDSAcceleratorComponent* BDSComponentFactory::CreateComponent(Element* elementIn,
                                       std::vector<GMAD::Element*> prevElementIn,
                                       std::vector<GMAD::Element*> nextElementIn)
{
  element      = elementIn;
  // vectors of all next/previous elements plus any thinmultipoles inbetween
  prevElements = prevElementIn;
  nextElements = nextElementIn;
  G4double angleIn  = 0.0;
  G4double angleOut = 0.0;
  G4bool registered = false;
  // Used for multiple instances of the same element but different poleface rotations.
  G4bool willModify = false;

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "named: \"" << element->name << "\"" << G4endl;  
#endif

  // set next/previous element to be last (and only non thinmultipole) element in the vector
  if (BDS::IsFinite(prevElements.size()))
    {prevElement = prevElements.back();}
  if (BDS::IsFinite(nextElements.size()))
    {nextElement = nextElements.back();}

  if (BDSAcceleratorComponentRegistry::Instance()->IsRegistered(element->name))
    {registered = true;}

  if (element->type == ElementType::_DRIFT)
    {
      // Match poleface from previous and next element
      angleIn  = (prevElement) ? ( prevElement->e2 * CLHEP::rad ) : 0.0;
      angleOut = (nextElement) ? ( nextElement->e1 * CLHEP::rad ) : 0.0;

      // Normal vector of rbend is from the magnet, angle of the rbend has to be
      // taken into account regardless of poleface rotation
      if (prevElement && (prevElement->type == ElementType::_RBEND))
	{angleIn += 0.5*(prevElement->angle);} // won't work if only field set TBC

      if (nextElement && (nextElement->type == ElementType::_RBEND))
	{angleOut += 0.5*nextElement->angle;}// won't work if only field set TBC

      //if drift has been modified at all
      if (BDS::IsFinite(angleIn) || BDS::IsFinite(angleOut))
      {willModify = true;}
    }
  else if (element->type == ElementType::_RBEND)
    {
      // angleIn and angleOut have to be multiplied by minus one for rbends for
      // some reason. Cannot figure out why yet.
      angleIn  = -1.0 * element->e1;
      angleOut = -1.0 * element->e2;

      if (nextElement && (nextElement->type == ElementType::_RBEND))
        {
          willModify = true;
          angleOut += 0.5*element->angle;
        }
      if (prevElement && (prevElement->type == ElementType::_RBEND))
        {
          willModify = true;
          angleIn += 0.5*element->angle;
        }
    }
  else if (element->type == ElementType::_SBEND)
    {
      angleIn = element->e1 - 0.5*element->angle;
      angleOut = element->e2 - 0.5*element->angle;
    }
  else if (element->type == ElementType::_THINMULT)
    {
      if (nextElement && (BDS::IsFinite(nextElement->e1)))
      {
        element->e1 = nextElement->e1 * CLHEP::rad;
        element->e2 = nextElement->e1 * CLHEP::rad;
        willModify  = true;
      }
      else if (prevElement && (BDS::IsFinite(prevElement->e2)))
      {
        element->e1 = prevElement->e2 * CLHEP::rad;
        element->e2 = prevElement->e2 * CLHEP::rad;
        willModify  = true;
      }
    }

  // check if the component already exists and return that
  // don't use registry for output elements since reliant on unique name
  if (registered && !willModify)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "using already manufactured component" << G4endl;
#endif
      return BDSAcceleratorComponentRegistry::Instance()->GetComponent(element->name);
    }

  BDSAcceleratorComponent* component = nullptr;
#ifdef BDSDEBUG
  G4cout << "BDSComponentFactory - creating " << element->type << G4endl;
#endif
  switch(element->type){
  case ElementType::_DRIFT:
    component = CreateDrift(angleIn, angleOut); break;
  case ElementType::_RF:
    component = CreateRF(); break;
  case ElementType::_SBEND:
    component = CreateSBend(angleIn,angleOut); break;
  case ElementType::_RBEND:
    component = CreateRBend(angleIn, angleOut); break;
  case ElementType::_HKICK:
    component = CreateKicker(false); break;
  case ElementType::_VKICK:
    component = CreateKicker(true); break;
  case ElementType::_QUAD:
    component = CreateQuad(); break;
  case ElementType::_SEXTUPOLE:
    component = CreateSextupole(); break;
  case ElementType::_OCTUPOLE:
    component = CreateOctupole(); break;
  case ElementType::_DECAPOLE:
    component = CreateDecapole(); break;
  case ElementType::_MULT:
    component = CreateMultipole(); break;
  case ElementType::_THINMULT:
    component = CreateThinMultipole(); break;
  case ElementType::_ELEMENT:
    component = CreateElement(); break;
  case ElementType::_SOLENOID:
    component = CreateSolenoid(); break; 
  case ElementType::_ECOL:
    component = CreateEllipticalCollimator(); break; 
  case ElementType::_RCOL:
    component = CreateRectangularCollimator(); break; 
  case ElementType::_MUSPOILER:    
    component = CreateMuSpoiler(); break;
  case ElementType::_DEGRADER:
    component = CreateDegrader(); break;
  case ElementType::_LASER:
    component = CreateLaser(); break; 
  case ElementType::_SCREEN:
    component = CreateScreen(); break; 
  case ElementType::_AWAKESCREEN:
    component = CreateAwakeScreen(); break; 
  case ElementType::_TRANSFORM3D:
    component = CreateTransform3D(); break;
    
    // common types, but nothing to do here
  case ElementType::_MARKER:
  case ElementType::_LINE:
  case ElementType::_REV_LINE:
    component = nullptr;
    break;
  default:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "type: " << element->type << G4endl; 
#endif
    G4cerr << __METHOD_NAME__ << "unknown type" << G4endl;
    exit(1);
    break;
  }

  // note this test will only be reached (and therefore the component registered)
  // if it both didn't exist and has been constructed
  if (component)
    {
      component->SetBiasVacuumList(element->biasVacuumList);
      component->SetBiasMaterialList(element->biasMaterialList);
      component->SetPrecisionRegion(element->precisionRegion);
      component->Initialise();
      // register component and memory
      BDSAcceleratorComponentRegistry::Instance()->RegisterComponent(component,willModify);
    }
  
  return component;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateTeleporter(const G4ThreeVector teleporterDetla)
{
  // This relies on things being added to the beamline immediately
  // after they've been created
  G4double teleporterLength = BDSGlobalConstants::Instance()->TeleporterLength() - 1e-8;

  if (teleporterLength < 10*G4GeometryTolerance::GetInstance()->GetSurfaceTolerance())
    {
      G4cout << G4endl << __METHOD_NAME__ << "WARNING - no space to put in teleporter - skipping it!" << G4endl << G4endl;
      return nullptr;
    }
  
  G4String name = "teleporter";
#ifdef BDSDEBUG
  G4cout << "---->creating Teleporter,"
	 << " name = " << name
	 << ", l = " << teleporterLength/CLHEP::m << "m"
	 << G4endl;
#endif

  return( new BDSTeleporter(name,
			    teleporterLength,
			    teleporterDetla));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateDrift(G4double angleIn,
							  G4double angleOut)
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

#ifdef BDSDEBUG
  G4cout << "---->creating Drift,"
	 << " name= " << element->name
	 << " l= " << element->l << "m"
	 << G4endl;
#endif

  // Beampipeinfo needed here to get aper1 for check.
  BDSBeamPipeInfo* beamPipeInfo = PrepareBeamPipeInfo(element, angleIn, angleOut);

  G4double projLengthIn  = 2.0 * std::abs(tan(angleIn))  * (beamPipeInfo->aper1*CLHEP::mm) ;
  G4double projLengthOut = 2.0 * std::abs(tan(angleOut)) * (beamPipeInfo->aper1*CLHEP::mm) ;
  G4double elementLength = element->l * CLHEP::m;

  if (projLengthIn > elementLength)
    {
      G4cerr << __METHOD_NAME__ << "Drift " << element->name
	     << " is too short for outgoing Poleface angle from "
	     << prevElement->name << G4endl;
      exit(1);
    }
  if (projLengthOut > elementLength)
    {
      G4cerr << __METHOD_NAME__ << "Drift " << element->name
	     << " is too short for incoming Poleface angle from "
	     << nextElement->name << G4endl;
      exit(1);
    }

  return (new BDSDrift( element->name,
			element->l*CLHEP::m,
			beamPipeInfo));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateRF()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSIntegratorType intType = BDS::Integrator(integratorSet, BDSFieldType::rfcavity);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::rfcavity,
					       brho,
					       intType,
					       nullptr,
					       true,
					       G4Transform3D(),
					       PrepareCavityModelInfo(element));

  return new BDSCavityRF(element->name,
			 element->l*CLHEP::m,
			 vacuumField);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateSBend(G4double angleIn,
                                    G4double angleOut)
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  PoleFaceRotationsNotTooLarge(element);  // check if poleface is not too large

  // require drift next to non-zero poleface or sbend with matching poleface
  if (BDS::IsFinite(element->e1))
    {
      if (
	  prevElement &&
	  prevElement->type != ElementType::_DRIFT &&
          prevElement->type != ElementType ::_THINMULT &&
          !(prevElement->type == ElementType::_SBEND && !BDS::IsFinite(prevElement->e2 + element->e1) )
	  )
	{
	  G4cerr << __METHOD_NAME__ << "SBend with non-zero incoming poleface requires previous element "
		 << "to be a Drift or SBend with opposite outcoming poleface" << G4endl;
	  exit(1);
	}
    }

  if (BDS::IsFinite(element->e2))
    {
      if (
	  nextElement &&
	  nextElement->type != ElementType::_DRIFT &&
          nextElement->type != ElementType ::_THINMULT &&
	  !(nextElement->type == ElementType::_SBEND && !BDS::IsFinite(nextElement->e1 + element->e2) )
	  )
	{
	  G4cerr << __METHOD_NAME__ << "SBend with non-zero incoming poleface requires next element to "
		 << "be a Drift or SBend with opposite incoming poleface" << G4endl;
	  exit(1);
	}
    }

  G4double length = element->l * CLHEP::m;
  BDSMagnetStrength* st = new BDSMagnetStrength();
  if (BDS::IsFinite(element->B) && BDS::IsFinite(element->angle))
    {// both are specified and should be used - under or overpowered dipole by design
      (*st)["field"] = element->B;
      (*st)["angle"] = - element->angle;
    }
  else if (BDS::IsFinite(element->B))
    {// only B field - calculate angle
      G4double ffact = BDSGlobalConstants::Instance()->FFact();
      (*st)["field"] = element->B;
      (*st)["angle"] = (*st)["field"] * length * charge * ffact / brho;
    }
  else
    {// only angle - calculate B field
      G4double ffact = BDSGlobalConstants::Instance()->FFact();
      (*st)["angle"] = - element->angle;
      (*st)["field"] = - brho * (*st)["angle"] / length * charge * ffact / CLHEP::tesla / CLHEP::m;
    }
  //copy of angle
  G4double angle = (*st)["angle"];
  // Quadrupole component
  if (BDS::IsFinite(element->k1))
    {(*st)["k1"] = element->k1 / CLHEP::m2;}

#ifdef BDSDEBUG
  G4cout << "Angle " << (*st)["angle"] << G4endl;
  G4cout << "Field " << (*st)["field"] << G4endl;
#endif

  // Calculate number of sbends to split parent into
  G4int nSBends = CalculateNSBendSegments(element);

  //Zero angle bend only needs one element.
  std::string thename = element->name + "_1_of_1";

  // Single element if no poleface and zero bend angle or dontSplitSBends=1, therefore nSBends = 1
  if (!BDS::IsFinite(angle) || (nSBends == 1))
    {
      BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::dipole,
						   brho,
						   BDSIntegratorType::dipole,
						   st);
      // prepare one sbend segment
      BDSMagnet* oneBend = new BDSMagnet(BDSMagnetType::sectorbend,
					 thename,
					 length,
					 PrepareBeamPipeInfo(element, -angleIn, -angleOut),
					 PrepareMagnetOuterInfo(element, -angleIn, -angleOut),
					 vacuumField);
      
      oneBend->SetBiasVacuumList(element->biasVacuumList);
      oneBend->SetBiasMaterialList(element->biasMaterialList);

      return oneBend;
    }
  else  //Otherwise, create line of sbend segments
  {return CreateSBendLine(element, nSBends, st);}
}

BDSLine* BDSComponentFactory::CreateSBendLine(Element*           element,
					      G4int              nSBends,
					      BDSMagnetStrength* st)
{
  BDSLine* sbendline  = new BDSLine(element->name);
  G4double length     = element->l*CLHEP::m;
  // prepare one name for all that makes sense
  std::string thename = element->name + "_1_of_" + std::to_string(nSBends);
  //calculate their angles and length
  G4double semiangle  = -element->angle / (G4double) nSBends;
  G4double semilength = length / (G4double) nSBends;
  G4double angleIn    = element->e1*CLHEP::rad;
  G4double angleOut   = element->e2*CLHEP::rad;
  G4double rho        = element->l*CLHEP::m/element->angle;
  G4double angle      = element->angle;

  BDSMagnetType magType = BDSMagnetType::sectorbend;
  // check magnet outer info
  BDSMagnetOuterInfo* magnetOuterInfoCheck = PrepareMagnetOuterInfo(element,angleIn,angleOut);
  CheckBendLengthAngleWidthCombo(semilength, semiangle, magnetOuterInfoCheck->outerDiameter, thename);
  // clean up
  delete magnetOuterInfoCheck;

  // angle increment for sbend elements with poleface rotation(s) specified
  G4double deltastart = -element->e1/(0.5*(nSBends-1));
  G4double deltaend   = -element->e2/(0.5*(nSBends-1));

  // first element should be fringe if poleface specified
  if (BDS::IsFinite(angleIn) && includeFringe)
    {
      BDSMagnetStrength* fringeStIn  = new BDSMagnetStrength();
      (*fringeStIn)["field"]         = (*st)["field"];
      (*fringeStIn)["length"]        = thinElementLength;
      (*fringeStIn)["angle"]         = -thinElementLength/rho;
      (*fringeStIn)["polefaceangle"] = element->e1;
      thename                        = element->name + "_e1_fringe";
      angle                          = element->e1 + 0.5*((*fringeStIn)["angle"]);
      BDSMagnet* startfringe = CreateDipoleFringe(element, angle, thename, magType, fringeStIn);
      sbendline->AddComponent(startfringe);
    }

  for (int i = 0; i < nSBends; ++i)
    {
      thename = element->name + "_"+std::to_string(i+1)+"_of_" + std::to_string(nSBends);

      // subtract thinElementLength from first and last elements if fringe & poleface specified
      length = semilength;
      if ((BDS::IsFinite(element->e1)) && (i == 0) && includeFringe)
        {length -= thinElementLength;}
      if ((BDS::IsFinite(element->e2)) && (i == nSBends-1) && includeFringe)
        {length -= thinElementLength;}
      semiangle = -length/rho;

      // Default angles for all segments
      angleIn = -semiangle*0.5;
      angleOut = -semiangle*0.5;

      // Input and output angles added to or subtracted from the default as appropriate
      // Note: case of i == 0.5*(nSBends-1) is just the default central wedge.
      // More detailed methodology/reasons in developer manual
      if ((BDS::IsFinite(element->e1))||(BDS::IsFinite(element->e2)))
        {
          if (i < 0.5*(nSBends-1))
            {
              angleIn -= (element->e1 + (i*deltastart));
              angleOut -= ((0.5*(nSBends-3)-i)*deltastart);
            }
          else if (i > 0.5*(nSBends-1))
            {
              angleIn  +=  (0.5*(nSBends+1)-i)*deltaend;
              angleOut += -(0.5*(nSBends-1)-i)*deltaend;
            }
        }
      if ((BDS::IsFinite(element->e1)) && (i == 0) && includeFringe)
      {angleIn += thinElementLength/rho;}
      if ((BDS::IsFinite(element->e2)) && (i == nSBends-1) && includeFringe)
      {angleOut += thinElementLength/rho;}

      // Check for intersection of angled faces.
      G4double intersectionX = BDS::CalculateFacesOverlapRadius(angleIn,angleOut,semilength);
      BDSMagnetOuterInfo* magnetOuterInfo = PrepareMagnetOuterInfo(element,angleIn,angleOut);
      G4double magnetRadius= 0.625*magnetOuterInfo->outerDiameter*CLHEP::mm;
      // Every geometry type has a completely arbitrary factor of 1.25 except cylindrical
      if (magnetOuterInfo->geometryType == BDSMagnetGeometryType::cylindrical)
	{magnetRadius= 0.5*magnetOuterInfo->outerDiameter*CLHEP::mm;}

      //Check if intersection is within radius
      if ((BDS::IsFinite(intersectionX)) && (std::abs(intersectionX) < magnetRadius))
        {
          G4cerr << __METHOD_NAME__ << "Angled faces of element "<< thename
		 << " intersect within the magnet radius." << G4endl;
          exit(1);
        }

      BDSMagnetStrength* stSemi = new BDSMagnetStrength(*st); // copy field strength - ie B
      (*stSemi)["length"] = length;
      (*stSemi)["angle"]  = semiangle;  // override copied length and angle

      BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::dipole,
						   brho,
						   BDSIntegratorType::dipole,
						   stSemi);

      BDSMagnet* oneBend = new BDSMagnet(magType,
					 thename,
					 length,
					 PrepareBeamPipeInfo(element, angleIn, angleOut),
					 magnetOuterInfo,
					 vacuumField,
					 semiangle);

      oneBend->SetBiasVacuumList(element->biasVacuumList);
      oneBend->SetBiasMaterialList(element->biasMaterialList);
      sbendline->AddComponent(oneBend);

#ifdef BDSDEBUG
  G4cout << "---->creating sbend line,"
     << " element= " << thename
	 << " angleIn= " << angleIn
	 << " angleOut= " << angleOut << "m"
	 << G4endl;
#endif
  }
  //Last element should be fringe if poleface specified
  if (BDS::IsFinite(element->e2) && includeFringe)
    {
      BDSMagnetStrength* fringeStOut  = new BDSMagnetStrength();
      (*fringeStOut)["angle"]         = -thinElementLength/rho;
      (*fringeStOut)["field"]         = (*st)["field"];
      (*fringeStOut)["polefaceangle"] = element->e2;
      (*fringeStOut)["length"]        = thinElementLength;
      angle                           = element->e2+ 0.5*((*fringeStOut)["angle"]);
      thename                         = element->name + "_e2_fringe";

      BDSMagnet* endfringe = CreateDipoleFringe(element, -angle, thename, magType, fringeStOut);
      sbendline->AddComponent(endfringe);
    }
  return sbendline;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateRBend(G4double angleIn,
							  G4double angleOut)
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  PoleFaceRotationsNotTooLarge(element);
  
  // require drift next to non-zero poleface or rbend with matching poleface
  if (BDS::IsFinite(element->e1))
    {
      if (
	  prevElement &&
	  prevElement->type != ElementType::_DRIFT &&
          prevElement->type != ElementType ::_THINMULT &&
          !(prevElement->type == ElementType::_RBEND && !BDS::IsFinite(prevElement->e2 + element->e1) )
	  )
	{
	  G4cerr << __METHOD_NAME__ << "RBend with non-zero incoming poleface requires previous element "
		 << "to be a Drift or RBend with opposite outcoming poleface" << G4endl;
	  exit(1);
	}
    }

  if (BDS::IsFinite(element->e2))
    {
      if (
	  nextElement &&
	  nextElement->type != ElementType::_DRIFT &&
          nextElement->type != ElementType ::_THINMULT &&
          !(nextElement->type == ElementType::_RBEND && !BDS::IsFinite(nextElement->e1 + element->e2) )
	  )
	{
	  G4cerr << __METHOD_NAME__ << "RBend with non-zero incoming poleface requires next "
		 << "element to be a Drift or RBend with opposite incoming poleface" << G4endl;
	  exit(1);
	}
    }

  BDSLine* rbendline  = new BDSLine(element->name);

  // calculate length of central straight length and edge sections
  // unfortunately, this has to be duplicated here as we need to
  // calculated the magnetic field length (less than the full length)
  // in case we need to calculate the field
  G4double outerRadius = PrepareOuterDiameter(element)*0.5;
  G4double angle       = element->angle;
  G4double length      = element->l*CLHEP::m;
  G4String thename     = element->name;
  G4double rho         = element->l*CLHEP::m/element->angle;

  CheckBendLengthAngleWidthCombo(length, angle, 2*outerRadius, element->name);

  BDSMagnetType magType = BDSMagnetType::rectangularbend;
  BDSMagnetStrength* st = new BDSMagnetStrength();
  if (BDS::IsFinite(element->B) && BDS::IsFinite(element->angle))
    {// both are specified and should be used - under or overpowered dipole by design
      (*st)["field"] = element->B;
      (*st)["angle"] = - element->angle;
    }
  else if (BDS::IsFinite(element->B))
    {// only B field - calculate angle
      G4double ffact = BDSGlobalConstants::Instance()->FFact();
      (*st)["field"] = element->B * CLHEP::tesla;
      (*st)["angle"] = charge * ffact * -2.0*asin(length*0.5 / (brho / (*st)["field"]));
    }
  else
    {// only angle - calculate B field
      G4double ffact = BDSGlobalConstants::Instance()->FFact();
      (*st)["angle"] = - element->angle;
      (*st)["field"] = - brho * (*st)["angle"] / length * charge * ffact / CLHEP::tesla / CLHEP::m;
    }
  // Quadrupole component
  if (BDS::IsFinite(element->k1))
    {(*st)["k1"] = element->k1 / CLHEP::m2;}

  // poleface angles
  G4double polefaceAngleIn = element->e1 + 0.5*(length-thinElementLength)/rho;
  G4double polefaceAngleOut = element->e2 + 0.5*(length-thinElementLength)/rho;

  // poleface angles modified if fringefields are
  if ((prevElement) && (prevElement->type == ElementType::_RBEND)){
    polefaceAngleIn -= 0.5*element->angle;
    angleIn += 0.5*(thinElementLength)/rho;}
  if ((nextElement) && (nextElement->type == ElementType::_RBEND)){
    polefaceAngleOut -= 0.5*element->angle;
    angleOut += 0.5*(thinElementLength)/rho;}

  // first element should be fringe if poleface specified
  if (BDS::IsFinite(element->e1) && includeFringe)
    {
      BDSMagnetStrength* fringeStIn  = new BDSMagnetStrength();
      (*fringeStIn)["field"]         = (*st)["field"];
      (*fringeStIn)["polefaceangle"] = element->e1;
      (*fringeStIn)["length"]        = thinElementLength;
      (*fringeStIn)["angle"]         = -thinElementLength/rho;
      thename                        = element->name + "_e1_fringe";
      angle                          = polefaceAngleIn;

              BDSMagnet* startfringe = CreateDipoleFringe(element, angle, thename, magType, fringeStIn);
      rbendline->AddComponent(startfringe);
    }

    // subtract thinElementLength from main rbend element if fringe & poleface(s) specified
  if (BDS::IsFinite(element->e1) && includeFringe)
    {length -= thinElementLength;}
  if (BDS::IsFinite(element->e2) && includeFringe)
    {length -= thinElementLength;}
  angle = -length/rho;

  // override copied length and angle
  (*st)["length"] = length;
  (*st)["angle"]  = angle;

  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::dipole,
					       brho,
					       BDSIntegratorType::dipole,
					       st);

  BDSMagnet* oneBend = new BDSMagnet(magType,
		       element->name,
		       length,
		       PrepareBeamPipeInfo(element, angleIn, angleOut),
		       PrepareMagnetOuterInfo(element, angleIn, angleOut),
		       vacuumField,
		       angle,
		       nullptr);

  rbendline->AddComponent(oneBend);

  //Last element should be fringe if poleface specified
  if (BDS::IsFinite(element->e2) && includeFringe)
    {
      BDSMagnetStrength* fringeStOut  = new BDSMagnetStrength();
      (*fringeStOut)["field"]         = (*st)["field"];
      (*fringeStOut)["polefaceangle"] = element->e2;
      (*fringeStOut)["length"]        = thinElementLength;
      (*fringeStOut)["angle"]         = -thinElementLength/rho;
      thename                         = element->name + "_e2_fringe";
      angle                           = polefaceAngleOut;

      BDSMagnet* endfringe = CreateDipoleFringe(element, -angle, thename, magType, fringeStOut);
      rbendline->AddComponent(endfringe);
    }
  return rbendline;
}

BDSMagnet* BDSComponentFactory::CreateDipoleFringe(Element* element,
                                    G4double angle,
                                    G4String name,
                                    BDSMagnetType magType,
                                    BDSMagnetStrength* st)
{
  BDSBeamPipeInfo* beamPipeInfo = PrepareBeamPipeInfo(element, -angle, angle);
  BDSMagnetOuterInfo* magnetOuterInfo = PrepareMagnetOuterInfo(element, -angle, angle);
  magnetOuterInfo->geometryType = BDSMagnetGeometryType::none;

  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::dipole,
					       brho,
					       BDSIntegratorType::fringe,
					       st);

  return new BDSMagnet(magType,
		       name,
               (*st)["length"],
		       beamPipeInfo,
		       magnetOuterInfo,
		       vacuumField,
               (*st)["angle"],
		       nullptr);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateKicker(G4bool isVertical)
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  G4double length = element->l*CLHEP::m;
  
  // magnetic field
  if(BDS::IsFinite(element->B))
    {
      G4double ffact = BDSGlobalConstants::Instance()->FFact();
      (*st)["field"] = - brho * element->angle / length * charge * ffact / CLHEP::tesla / CLHEP::m;
    }
  G4Transform3D fieldRotation = G4Transform3D();
  
  BDSMagnetType t = BDSMagnetType::hkicker;
  if (isVertical)
    {
      t = BDSMagnetType::vkicker;
      fieldRotation = G4RotateZ3D(CLHEP::halfpi);
    }
  
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::dipole,
					       brho,
					       BDSIntegratorType::g4classicalrk4,
					       st,
					       true,
					       fieldRotation);
  
  return new BDSMagnet(t,
		       element->name,
		       element->l*CLHEP::m,
		       PrepareBeamPipeInfo(element),
		       PrepareMagnetOuterInfo(element),
		       vacuumField);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateQuad()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["k1"] = element->k1 / CLHEP::m2;
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::quadrupole,
					       brho,
					       BDSIntegratorType::quadrupole,
					       st);

  return new BDSMagnet(BDSMagnetType::quadrupole,
		       element->name,
		       element->l * CLHEP::m,
		       PrepareBeamPipeInfo(element),
		       PrepareMagnetOuterInfo(element),
		       vacuumField);
}  
  
BDSAcceleratorComponent* BDSComponentFactory::CreateSextupole()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["k2"] = element->k2 / CLHEP::m3;
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::sextupole,
					       brho,
					       BDSIntegratorType::sextupole,
					       st);

  return new BDSMagnet(BDSMagnetType::sextupole,
		       element->name,
		       element->l * CLHEP::m,
		       PrepareBeamPipeInfo(element),
		       PrepareMagnetOuterInfo(element),
		       vacuumField);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateOctupole()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["k3"] = element->k3 / (CLHEP::m3*CLHEP::m);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::octupole,
					       brho,
					       BDSIntegratorType::octupole,
					       st);

  return new BDSMagnet(BDSMagnetType::octupole,
		       element->name,
		       element->l * CLHEP::m,
		       PrepareBeamPipeInfo(element),
		       PrepareMagnetOuterInfo(element),
		       vacuumField);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateDecapole()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["k4"] = element->k4 / (CLHEP::m3*CLHEP::m2);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::decapole,
					       brho,
					       BDSIntegratorType::decapole,
					       st);
  

  return new BDSMagnet(BDSMagnetType::decapole,
		       element->name,
		       element->l * CLHEP::m,
		       PrepareBeamPipeInfo(element),
		       PrepareMagnetOuterInfo(element),
		       vacuumField);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateMultipole()
{
 if(!HasSufficientMinimumLength(element))
    {return nullptr;}

 BDSMagnetStrength* st = new BDSMagnetStrength();
 std::list<double>::iterator kn = element->knl.begin();
 std::list<double>::iterator ks = element->ksl.begin();
 std::vector<G4String> normKeys = st->NormalComponentKeys();
 std::vector<G4String> skewKeys = st->SkewComponentKeys();
 std::vector<G4String>::iterator nkey = normKeys.begin();
 std::vector<G4String>::iterator skey = skewKeys.begin();
 for (; kn != element->knl.end(); kn++, ks++, nkey++, skey++)
   {
     (*st)[*nkey] = (*kn) / element->l;
     (*st)[*skey] = (*ks) / element->l;
   }
 BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::multipole,
					       brho,
					       BDSIntegratorType::g4classicalrk4,
					       st);

 return new BDSMagnet(BDSMagnetType::multipole,
		      element->name,
		      element->l * CLHEP::m,
		      PrepareBeamPipeInfo(element),
		      PrepareMagnetOuterInfo(element),
		      vacuumField,
		      (*st)["angle"]); // multipole could bend beamline
}

BDSAcceleratorComponent* BDSComponentFactory::CreateThinMultipole()
  {
 if(!HasSufficientMinimumLength(element))
    {return nullptr;}

 BDSMagnetStrength* st = new BDSMagnetStrength();
 std::list<double>::iterator kn = element->knl.begin();
 std::list<double>::iterator ks = element->ksl.begin();
 std::vector<G4String> normKeys = st->NormalComponentKeys();
 std::vector<G4String> skewKeys = st->SkewComponentKeys();
 std::vector<G4String>::iterator nkey = normKeys.begin();
 std::vector<G4String>::iterator skey = skewKeys.begin();

 //Don't divide by element length, keep strengths as knl/ksl
 for (; kn != element->knl.end(); kn++, ks++, nkey++, skey++)
   {
     (*st)[*nkey] = (*kn) ;
     (*st)[*skey] = (*ks) ;
   }

 BDSBeamPipeInfo* beamPipeInfo = PrepareBeamPipeInfo(element, -element->e1, element->e2);
 BDSMagnetOuterInfo* magnetOuterInfo = PrepareMagnetOuterInfo(element, -element->e1, element->e2);
 magnetOuterInfo->geometryType = BDSMagnetGeometryType::none;

 BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::multipole,
					       brho,
					       BDSIntegratorType::multipole,
					       st);

 BDSMagnet* thinMultipole =  new BDSMagnet(BDSMagnetType::multipole,
		      element->name,
		      thinElementLength,
                      beamPipeInfo,
                      magnetOuterInfo,
		      vacuumField,
                      0,
		      nullptr);

  thinMultipole->SetExtent(BDSExtent(beamPipeInfo->aper1,beamPipeInfo->aper1,thinElementLength*0.5));

  return thinMultipole;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateElement()
{
  if(!HasSufficientMinimumLength(element)) 
    {return nullptr;}

  if(!BDS::IsFinite(element->outerDiameter))
    {
      G4cerr << __METHOD_NAME__ << "\"outerDiameter\" must be set for component named \""
	     << element->name << "\"" << G4endl;
      exit(1);
    }

  G4ThreeVector bFieldOffset = G4ThreeVector(element->bmapXOffset * CLHEP::m,
					     element->bmapYOffset * CLHEP::m,
					     element->bmapZOffset * CLHEP::m);
#ifdef BDSDEBUG 
  G4cout << "---->creating Element,"
	 << " name = " << element->name
	 << " l = " << element->l << "m"
	 << " outerDiameter = "  << element->outerDiameter << "m"
	 << " B Field Offset = " << bFieldOffset << " m"
	 << " precision region " << element->precisionRegion
	 << G4endl;
#endif

  return (new BDSElement(element->name,
			 element->l * CLHEP::m,
			 element->outerDiameter * CLHEP::m,
			 element->geometryFile,
			 element->bmapFile,
			 bFieldOffset));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateSolenoid()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  if (BDS::IsFinite(element->B))
    {
      (*st)["field"] = element->B * CLHEP::tesla;
      (*st)["ks"]    = (*st)["field"] / brho / CLHEP::m;
    }
  else
    {
      (*st)["field"] = (element->ks / CLHEP::m) * brho;
      (*st)["ks"]    = element->ks / CLHEP::m;
    }
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::solenoid,
					       brho,
					       BDSIntegratorType::solenoid,
					       st);

  return new BDSMagnet(BDSMagnetType::solenoid,
		       element->name,
		       element->l*CLHEP::m,
		       PrepareBeamPipeInfo(element),
		       PrepareMagnetOuterInfo(element),
		       vacuumField);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateRectangularCollimator()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

#ifdef BDSDEBUG 
  G4cout << "--->creating " << element->type << ","
	 << " name = " << element->name  << ","
	 << " x half aperture = " << element->xsize <<" m,"
	 << " y half aperture = " << element->ysize <<" m,"
	 << " material = \"" << element->material << "\""
	 << G4endl;
#endif
  
  return new BDSCollimatorRectangular(element->name,
				      element->l*CLHEP::m,
				      element->outerDiameter*CLHEP::m,
				      element->xsize*CLHEP::m,
				      element->ysize*CLHEP::m,
				      element->xsizeOut*CLHEP::m,
				      element->ysizeOut*CLHEP::m,
				      G4String(element->material),
				      G4String(element->vacuumMaterial),
				      PrepareColour(element, "collimator"));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateEllipticalCollimator()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

#ifdef BDSDEBUG 
  G4cout << "--->creating " << element->type << ","
	 << " name = " << element->name 
	 << " x half aperture = " << element->xsize <<" m,"
	 << " y half aperture = " << element->ysize <<" m,"
	 << " material = \"" << element->material << "\""
	 << G4endl;
#endif
  
  return new BDSCollimatorElliptical(element->name,
				     element->l*CLHEP::m,
				     element->outerDiameter*CLHEP::m,
				     element->xsize*CLHEP::m,
				     element->ysize*CLHEP::m,
				     element->xsizeOut*CLHEP::m,
				     element->ysizeOut*CLHEP::m,
				     G4String(element->material),
				     G4String(element->vacuumMaterial),
				     PrepareColour(element, "collimator"));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateMuSpoiler()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["field"] = element->B * CLHEP::tesla;
  BDSFieldInfo* outerField = new BDSFieldInfo(BDSFieldType::muonspoiler,
					      brho,
					      BDSIntegratorType::g4classicalrk4,
					      st);
    BDSFieldInfo* vacuumField = new BDSFieldInfo();

  return new BDSMagnet(BDSMagnetType::muonspoiler,
		       element->name,
		       element->l*CLHEP::m,
		       PrepareBeamPipeInfo(element),
		       PrepareMagnetOuterInfo(element),
		       vacuumField,
		       0,
		       outerField);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateDegrader()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

#ifdef BDSDEBUG
  G4cout << "---->creating degrader,"
	 << " name = "   << element->name
	 << " length = " << element->l
	 << G4endl;
#endif

  G4double degraderOffset;
  if ((element->materialThickness <= 0) && (element->degraderOffset <= 0))
    {
        G4cerr << __METHOD_NAME__ << "Error: Both \"materialThickness\" and \"degraderOffset\" are either undefined or <= 0" <<  G4endl;
        exit(1);
    }

  if ((element->materialThickness <= 0) && (element->degraderOffset > 0))
    {degraderOffset = element->degraderOffset*CLHEP::m;}
  else
    {
      //Width of wedge base
      G4double wedgeBasewidth = (element->l*CLHEP::m /element->numberWedges) - lengthSafety;
      
      //Angle between hypotenuse and height (in the triangular wedge face)
      G4double theta = atan(wedgeBasewidth / (2.0*element->wedgeLength*CLHEP::m));
      
      //Overlap distance of wedges
      G4double overlap = (element->materialThickness*CLHEP::m/element->numberWedges - wedgeBasewidth) * (sin(CLHEP::pi/2.0 - theta) / sin(theta));
      
      degraderOffset = overlap * -0.5;
    }
    
  return (new BDSDegrader(element->name,
			  element->l*CLHEP::m,
			  element->outerDiameter*CLHEP::m,
			  element->numberWedges,
			  element->wedgeLength*CLHEP::m,
			  element->degraderHeight*CLHEP::m,
			  degraderOffset,
			  element->material));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateLaser()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}
	
#ifdef BDSDEBUG 
  G4cout << "---->creating Laser,"
	 << " name= "<< element->name
	 << " l=" << element->l <<"m"
	 << " lambda= " << element->waveLength << "m"
	 << " xSigma= " << element->xsize << "m"
	 << " ySigma= " << element->ysize << "m"
	 << " xdir= " << element->xdir
	 << " ydir= " << element->ydir
	 << " zdir= " << element->zdir
	 << G4endl;
#endif

  G4double length = element->l*CLHEP::m;
  G4double lambda = element->waveLength*CLHEP::m;
	
  G4ThreeVector direction = G4ThreeVector(element->xdir,element->ydir,element->zdir);
  G4ThreeVector position  = G4ThreeVector(0,0,0);
	
  return (new BDSLaserWire(element->name, length, lambda, direction) );       
}

BDSAcceleratorComponent* BDSComponentFactory::CreateScreen()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}
	
#ifdef BDSDEBUG 
        G4cout << "---->creating Screen,"
               << " name= "<< element->name
               << " l=" << element->l/CLHEP::m<<"m"
               << " tscint=" << element->tscint/CLHEP::m<<"m"
               << " angle=" << element->angle/CLHEP::rad<<"rad"
               << " scintmaterial=" << "ups923a"//element->scintmaterial
               << " airmaterial=" << "vacuum"//element->airmaterial
               << G4endl;
#endif
	return (new BDSScintillatorScreen(element->name,
					  element->tscint*CLHEP::m,
					  (element->angle-0.78539816339)*CLHEP::rad,
					  "ups923a",
                                      BDSGlobalConstants::Instance()->VacuumMaterial()));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateAwakeScreen()
{	
#ifdef BDSDEBUG 
        G4cout << "---->creating Awake Screen,"
	       << "twindow = " << element->twindow*1e3/CLHEP::um << " um"
	       << "tscint = " << element->tscint*1e3/CLHEP::um << " um"
	       << "windowmaterial = " << element->windowmaterial << " um"
	       << "scintmaterial = " << element->scintmaterial << " um"
               << G4endl;
#endif
	return (new BDSAwakeScintillatorScreen(element->name,
					       element->scintmaterial,
					       element->tscint*1e3,
					       element->angle,
					       element->twindow*1e3,
					       element->windowmaterial));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateTransform3D()
{
	
#ifdef BDSDEBUG 
  G4cout << "---->creating Transform3d,"
	 << " name= " << element->name
	 << " xdir= " << element->xdir/CLHEP::m << "m"
	 << " ydir= " << element->ydir/CLHEP::m << "m"
	 << " zdir= " << element->zdir/CLHEP::m << "m"
	 << " phi= " << element->phi/CLHEP::rad << "rad"
	 << " theta= " << element->theta/CLHEP::rad << "rad"
	 << " psi= " << element->psi/CLHEP::rad << "rad"
	 << G4endl;
#endif
	
  return (new BDSTransform3D( element->name,
			      element->xdir *CLHEP::m,
			      element->ydir *CLHEP::m,
			      element->zdir *CLHEP::m,
			      element->phi *CLHEP::rad,
			      element->theta *CLHEP::rad,
			      element->psi *CLHEP::rad ) );
	
}

BDSAcceleratorComponent* BDSComponentFactory::CreateTerminator()
{
  G4String name   = "terminator";
  G4double length = BDSSamplerPlane::ChordLength();
#ifdef BDSDEBUG
    G4cout << "---->creating Terminator,"
	   << " name = " << name
	   << " l = "    << length / CLHEP::m << "m"
	   << G4endl;
#endif
  
  return new BDSTerminator("terminator", 
			   length);
}

G4bool BDSComponentFactory::HasSufficientMinimumLength(Element* element)
{
  if(element->l*CLHEP::m < 4*lengthSafety)
    {
      G4cerr << "---->NOT creating element, "
             << " name = " << element->name
             << ", LENGTH TOO SHORT:"
             << " l = " << element->l*CLHEP::um << "um"
             << G4endl;
      return false;
    }
  else
    {return true;}
}

void BDSComponentFactory::PoleFaceRotationsNotTooLarge(Element* element,
						       G4double maxAngle)
{
  if (std::abs(element->e1) > maxAngle)
    {
      G4cerr << __METHOD_NAME__ << "Pole face angle e1: " << element->e1 << " is greater than pi/4" << G4endl;
      exit(1);
    }
  if (std::abs(element->e2) > maxAngle)
    {
      G4cerr << __METHOD_NAME__ << "Pole face angle e2: " << element->e2 << " is greater than pi/4" << G4endl;
      exit(1);
    }
}

BDSMagnetOuterInfo* BDSComponentFactory::PrepareMagnetOuterInfo(Element const* element) const
{
  // input and output face angles
  G4double angleIn  = 0;
  G4double angleOut = 0;
  if (element->type == ElementType::_RBEND)
    {
      angleIn  = -1.0*element->e1*CLHEP::rad;
      angleOut = -1.0*element->e2*CLHEP::rad;
    }
  else if (element->type == ElementType::_SBEND)
    {
      angleIn  = (element->angle*0.5) + element->e1;
      angleOut = (element->angle*0.5) + element->e2;
    }
  return PrepareMagnetOuterInfo(element, angleIn, angleOut);
}

BDSMagnetOuterInfo* BDSComponentFactory::PrepareMagnetOuterInfo(Element const* element,
								const G4double angleIn,
								const G4double angleOut) const
{
  BDSMagnetOuterInfo* info = new BDSMagnetOuterInfo();

  // angle - we can't set here as we can't rely on the angle being specified in element
  // as only the field may be specified. Therefore, must be set in above CreateXXXX methods
  
  // name
  info->name = element->name;
  
  // magnet geometry type
  if (element->magnetGeometryType == "")
    {info->geometryType = BDSGlobalConstants::Instance()->GetMagnetGeometryType();}
  else
    {info->geometryType = BDS::DetermineMagnetGeometryType(element->magnetGeometryType);}

  // set face angles w.r.t. chord
  info->angleIn  = angleIn;
  info->angleOut = angleOut;
  
  // outer diameter
  G4double outerDiameter = element->outerDiameter*CLHEP::m;
  if (outerDiameter < 1e-6)
    {//outerDiameter not set - use global option as default
      outerDiameter = BDSGlobalConstants::Instance()->OuterDiameter();
    }
  info->outerDiameter = outerDiameter;

  // outer material
  G4Material* outerMaterial;
  if(element->outerMaterial == "")
    {
      G4String defaultMaterialName = BDSGlobalConstants::Instance()->OuterMaterialName();
      outerMaterial = BDSMaterials::Instance()->GetMaterial(defaultMaterialName);
    }
  else
    {outerMaterial = BDSMaterials::Instance()->GetMaterial(element->outerMaterial);}
  info->outerMaterial = outerMaterial;
  
  return info;
}

G4double BDSComponentFactory::PrepareOuterDiameter(Element const* element) const
{
  G4double outerDiameter = element->outerDiameter*CLHEP::m;
  if (outerDiameter < 1e-6)
    {//outerDiameter not set - use global option as default
      outerDiameter = BDSGlobalConstants::Instance()->OuterDiameter();
    }
  // returns in metres
  return outerDiameter;
}

BDSBeamPipeInfo* BDSComponentFactory::PrepareBeamPipeInfo(Element const* element,
							  const G4double angleIn,
							  const G4double angleOut) const
{
  BDSBeamPipeInfo* defaultModel = BDSGlobalConstants::Instance()->GetDefaultBeamPipeModel();
  BDSBeamPipeInfo* info = new BDSBeamPipeInfo(defaultModel,
					      element->apertureType,
					      element->aper1 * CLHEP::m,
					      element->aper2 * CLHEP::m,
					      element->aper3 * CLHEP::m,
					      element->aper4 * CLHEP::m,
					      element->vacuumMaterial,
					      element->beampipeThickness * CLHEP::m,
					      element->beampipeMaterial,
					      angleIn,
					      angleOut);
  return info;
}

G4int BDSComponentFactory::CalculateNSBendSegments(Element const* element,
						   const G4double aperturePrecision) const
{
  //if maximum distance between arc path and straight path larger than 1mm, split sbend into N chunks,
  //this also works when maximum distance is less than 1mm as there will just be 1 chunk!

  G4double length = element->l*CLHEP::m;
  // from formula: L/2 / N tan (angle/N) < precision. (L=physical length)
  // add poleface rotations onto angle as absolute number (just to be safe)
  G4double totalAngle = std::abs(element->angle) + std::abs(element->e1) + std::abs(element->e2);
  G4int nSBends = (G4int) ceil(std::sqrt(length*totalAngle/2/aperturePrecision));
  if (nSBends==0)
    {nSBends = 1;} // can happen in case angle = 0
  if (BDSGlobalConstants::Instance()->DontSplitSBends())
    {nSBends = 1;}  // use for debugging
  if (nSBends % 2 == 0)
    {nSBends += 1;} // always have odd number of poles for poleface rotations
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " splitting sbend into " << nSBends << " sbends" << G4endl;
#endif
  return nSBends;
}

BDSTiltOffset* BDSComponentFactory::CreateTiltOffset(Element const* element) const
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "offsetX,Y: " << element->offsetX << " " << element->offsetY << " tilt: " << element->tilt << G4endl;
#endif
  G4double xOffset = element->offsetX * CLHEP::m;
  G4double yOffset = element->offsetY * CLHEP::m;
  G4double tilt    = element->tilt;

  return new BDSTiltOffset(xOffset, yOffset, tilt);
}

void BDSComponentFactory::CheckBendLengthAngleWidthCombo(G4double chordLength,
							 G4double angle,
							 G4double outerDiameter,
							 G4String name)
{
  G4double radiusFromAngleLength =  std::abs(chordLength / angle); // s = r*theta -> r = s/theta
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "radius from angle and length: " << radiusFromAngleLength << G4endl;
#endif
  if (outerDiameter > 2*radiusFromAngleLength)
    {
      G4cerr << "Error: the combination of length, angle and outerDiameter in element named \""
	     << name
	     << "\" will result in overlapping faces!" << G4endl << "Please correct!" << G4endl;
      exit(1);
    }
}

void BDSComponentFactory::PrepareCavityModels()
{
  for (auto model : BDSParser::Instance()->GetCavityModels())
    {
      auto info = new BDSCavityInfo(BDS::DetermineCavityType(model.type),
				    nullptr, //construct without material as stored in element
				    nullptr,
				    model.eField*CLHEP::volt / CLHEP::m,
				    model.frequency*CLHEP::hertz,
				    model.phase,
				    model.irisRadius*CLHEP::m,
				    model.thickness*CLHEP::m,
				    model.equatorRadius*CLHEP::m,
				    model.halfCellLength*CLHEP::m,
				    model.numberOfPoints,
				    model.numberOfCells,
				    model.equatorEllipseSemiAxis*CLHEP::m,
				    model.irisHorizontalAxis*CLHEP::m,
				    model.irisVerticalAxis*CLHEP::m,
				    model.tangentLineAngle);
      
      cavityInfos[model.name] = info;
    }
}

BDSCavityInfo* BDSComponentFactory::PrepareCavityModelInfo(Element const* element) const
{
  // If the cavity model name (identifier) has been defined, return a *copy* of
  // that model - so that the component will own that info object.
  auto result = cavityInfos.find(element->cavityModel);
  if (result == cavityInfos.end())
    {
      G4cout << "Unknown cavity model identifier \"" << element->cavityModel << "\" - please define it" << G4endl;
      exit(1);
    }

  // ok to use compiler provided copy constructor as doesn't own materials
  // which are the only pointers in this class
  BDSCavityInfo* info = new BDSCavityInfo(*(result->second));
  // update materials in info with valid materials - only element has material info
  if (!element->material.empty())
    {info->material       = BDSMaterials::Instance()->GetMaterial(element->material);}
  else
    {
      G4cout << "ERROR: Cavity material is not defined for cavity \"" << element->name << "\" - please define it" << G4endl;
      exit(1);
    }
  if(!element->vacuumMaterial.empty())
    {info->vacuumMaterial = BDSMaterials::Instance()->GetMaterial(element->vacuumMaterial);}
  else
    {info->vacuumMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->VacuumMaterial());}

  return info;
}

G4String BDSComponentFactory::PrepareColour(Element const* element, const G4String fallback) const
{
  G4String colour = element->colour;
  if (colour == "")
    {colour = fallback;}
  return colour;
}
