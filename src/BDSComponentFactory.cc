#include "BDSComponentFactory.hh"

// elements
#include "BDSAwakeScintillatorScreen.hh"
#include "BDSCavityRF.hh"
#include "BDSCollimatorElliptical.hh"
#include "BDSCollimatorRectangular.hh"
#include "BDSDegrader.hh"
#include "BDSDrift.hh"
#include "BDSElement.hh"
#include "BDSKicker.hh"
#include "BDSLaserWire.hh"
#include "BDSLine.hh"
#include "BDSMuSpoiler.hh"
#include "BDSOctupole.hh"
#include "BDSDecapole.hh"
#include "BDSQuadrupole.hh"
#include "BDSRBend.hh"
#include "BDSSamplerPlane.hh"
#include "BDSScintillatorScreen.hh"
#include "BDSSectorBend.hh"
#include "BDSSextupole.hh"
#include "BDSSolenoid.hh"
#include "BDSTerminator.hh"
#include "BDSTeleporter.hh"
#include "BDSTiltOffset.hh"
#include "BDSMultipole.hh"
#include "BDSTransform3D.hh"

// general
#include "BDSAcceleratorComponentRegistry.hh"
#include "BDSBeamPipeType.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSCavityInfo.hh"
#include "BDSCavityType.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetType.hh"
#include "BDSMagnetGeometryType.hh"
#include "BDSParser.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4GeometryTolerance.hh"

#include "parser/elementtype.h"
#include "parser/cavitymodel.h"

#include <cmath>
#include <string>
using namespace GMAD;


BDSComponentFactory::BDSComponentFactory()
{
  lengthSafety = BDSGlobalConstants::Instance()->LengthSafety();
  //
  // compute magnetic rigidity brho
  // formula: B(Tesla)*rho(m) = p(GeV)/(0.299792458 * |charge(e)|)
  //
  // charge (in e units)
  charge = BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGCharge();
  // momentum (in GeV/c)

  G4double momentum = BDSGlobalConstants::Instance()->BeamMomentum()/CLHEP::GeV;
  // rigidity (in T*m)
  brho = BDSGlobalConstants::Instance()->FFact()*( momentum / 0.299792458);
  
  // rigidity (in Geant4 units)
  brho *= (CLHEP::tesla*CLHEP::m);

  G4cout << "Rigidity (Brho) : "<< std::abs(brho)/(CLHEP::tesla*CLHEP::m) << " T*m"<<G4endl;

  // prepare rf cavity model info from parser
  PrepareCavityModels();
}

BDSComponentFactory::~BDSComponentFactory()
{
  for(auto info : cavityInfos)
    {delete info.second;}
}

BDSAcceleratorComponent* BDSComponentFactory::CreateComponent(Element* elementIn,
							      Element* prevElementIn,
							      Element* nextElementIn)
{
  element     = elementIn;
  prevElement = prevElementIn;
  nextElement = nextElementIn;
  G4double angleIn  = 0.0;
  G4double angleOut = 0.0;
  G4bool registered    = false;
  // Used for multiple instances of the same element but different poleface rotations.
  // Ie only a drift is modified to match the pole face rotation of a magnet.
  G4bool willModify = false;

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "named: \"" << element->name << "\"" << G4endl;  
#endif

  if (BDSAcceleratorComponentRegistry::Instance()->IsRegistered(element->name))
    {registered = true;}

  if (element->type == ElementType::_DRIFT)
    {
      // Match poleface from previous and next element
      angleIn = (prevElement) ? ( prevElement->e2 * CLHEP::rad ) : 0.0;
      angleOut = (nextElement) ? ( nextElement->e1 * CLHEP::rad ) : 0.0;

      // Normal vector of rbend is from the magnet, angle of the rbend has to be
      // taken into account regardless of poleface rotation
      if (prevElement && (prevElement->type == ElementType::_RBEND))
	{angleIn += -0.5*(prevElement->angle);}

      if (nextElement && (nextElement->type == ElementType::_RBEND))
	{angleOut += 0.5*nextElement->angle;}

      //if drift has been modified at all
      if (BDS::IsFinite(angleIn) || BDS::IsFinite(angleOut))
	{willModify = true;}
    }
  else if (element->type == ElementType::_RBEND)
    {
      // angleIn and angleOut have to be multiplied by minus one for rbends for
      // some reason. Cannot figure out why yet.
      angleIn = -1.0 * element->e1;
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

  // check if the component already exists and return that
  // don't use registry for output elements since reliant on unique name
  if (registered && !willModify)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "using already manufactured component" << G4endl;
#endif
      return BDSAcceleratorComponentRegistry::Instance()->GetComponent(element->name);
    }

  // Create normal vectors for drifts
  std::pair<G4ThreeVector,G4ThreeVector> faces = BDS::CalculateFaces(angleIn, angleOut);

  // current element tilt
  G4double currentTilt  = element->tilt * CLHEP::rad;
  G4double prevTilt = 0;
  G4double nextTilt     = 0;
  if (prevElement)
    {prevTilt = prevElement->tilt * CLHEP::rad;}
  if (nextElement)
    {nextTilt = nextElement->tilt * CLHEP::rad;}
  G4ThreeVector inputFaceNormal  = faces.first.rotateZ(prevTilt - currentTilt);
  G4ThreeVector outputFaceNormal = faces.second.rotateZ(nextTilt + currentTilt);
  
  BDSAcceleratorComponent* component = nullptr;
#ifdef BDSDEBUG
  G4cout << "BDSComponentFactory - creating " << element->type << G4endl;
#endif
  switch(element->type){
  case ElementType::_DRIFT:
    component = CreateDrift(inputFaceNormal, outputFaceNormal); break;
  case ElementType::_RF:
    component = CreateRF(); break;
  case ElementType::_SBEND:
    component = CreateSBend(angleIn,angleOut); break;
  case ElementType::_RBEND:
    component = CreateRBend(angleIn,angleOut); break;
  case ElementType::_HKICK:
    component = CreateHKick(); break;
  case ElementType::_VKICK:
    component = CreateVKick(); break;
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

BDSAcceleratorComponent* BDSComponentFactory::CreateTeleporter()
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
			      teleporterLength ));
  
}

BDSAcceleratorComponent* BDSComponentFactory::CreateDrift(G4ThreeVector inputFaceNormal,
							  G4ThreeVector outputFaceNormal)
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

#ifdef BDSDEBUG
  G4cout << "---->creating Drift,"
	 << " name= " << element->name
	 << " l= " << element->l << "m"
	 << G4endl;
#endif
  const G4double length = element->l*CLHEP::m;

  // Beampipeinfo needed here to get aper1 for check.
  BDSBeamPipeInfo* beamPipeInfo = PrepareBeamPipeInfo(element, inputFaceNormal,
						      outputFaceNormal);

  const BDSExtent indicativeExtent = beamPipeInfo->IndicativeExtent();
  G4bool facesWillIntersect = BDS::WillIntersect(inputFaceNormal, outputFaceNormal,
						 length, indicativeExtent, indicativeExtent);

  if (facesWillIntersect)
    {
      G4cerr << __METHOD_NAME__ << "Drift \"" << element->name
	     << "\" is too short for angled faces between \"";
      if (prevElement)
	{G4cerr << prevElement->name;}
      else
	{G4cerr << "none";}
      G4cerr << "\" and \"";
      if (nextElement)
	{G4cerr << nextElement->name;}
      else
	{G4cerr << "none";}
      G4cerr << "\"" << G4endl;
      exit(1);
    }

  return (new BDSDrift( element->name,
			length,
			beamPipeInfo));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateRF()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  return (new BDSCavityRF(element->name,
			  element->l*CLHEP::m,
			  element->gradient,
			  PrepareCavityModelInfo(element)));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateSBend(G4double angleIn,
                                    G4double angleOut)
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  PoleFaceRotationsNotTooLarge(element);  // check if poleface is not too large

  // require drift next to non-zero poleface or sbend with matching poleface
  if (BDS::IsFinite(element->e1) )
    {
      if (
	  prevElement &&
	  !(prevElement->type == ElementType::_DRIFT) &&
	  !(prevElement->type == ElementType::_SBEND && !BDS::IsFinite(prevElement->e2 + element->e1) )
	  )
	{
	  G4cerr << __METHOD_NAME__ << "SBend with non-zero incoming poleface requires previous element to be a Drift or SBend with opposite outcoming poleface" << G4endl;
	  exit(1);
	}
    }

  if (BDS::IsFinite(element->e2) )
    {
      if (
	  nextElement &&
	  !(nextElement->type == ElementType::_DRIFT) &&
	  !(nextElement->type == ElementType::_SBEND && !BDS::IsFinite(nextElement->e1 + element->e2) )
	  )
	{
	  G4cerr << __METHOD_NAME__ << "SBend with non-zero incoming poleface requires next element to be a Drift or SBend with opposite incoming poleface" << G4endl;
	  exit(1);
	}
    }

  // arc length
  G4double length = element->l*CLHEP::m;
  G4double magFieldLength = length; // initialise with this value
  
  // magnetic field
  // MAD conventions:
  // 1) a positive bend angle represents a bend to the right, i.e.
  // towards negative x values (even for negative charges??)
  // 2) a positive K1 = 1/|Brho| dBy/dx means horizontal focusing of
  // positive charges
  
  G4double bField;
  if(BDS::IsFinite(element->B) && BDS::IsFinite(element->angle))
    {// both are specified and should be used - under or overpowered dipole by design
      bField = element->B * CLHEP::tesla;
      element->angle *= -1; // convention
    }
  else if (BDS::IsFinite(element->B))
    {
      bField = element->B * CLHEP::tesla;
      G4double rho = brho/bField;
      //    element->angle  = - 2.0*asin(magFieldLength/2.0/rho);
      element->angle  = - magFieldLength/rho;
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "angle calculated from B(" << bField << ") : " << element->angle << G4endl;
#endif
    }
  else
    { // B = 0, but angle finite - calculate implied b field
      element->angle *= -1;
      //    bField = - 2 * brho * sin(element->angle/2.0) / magFieldLength;
      // charge in e units
      // multiply once more with ffact to not flip fields in bends
      bField = - brho * element->angle/magFieldLength * charge * BDSGlobalConstants::Instance()->FFact();
      element->B = bField/CLHEP::tesla;
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "B calculated from angle (" << element->angle << ") : " << bField << G4endl;
#endif
    }
  
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  // Brho is already in G4 units, but k1 is not -> multiply k1 by m^-2
  G4double bPrime = - brho * (element->k1 / CLHEP::m2);

  // Calculate number of sbends to split parent into
  G4int nSBends = CalculateNSBendSegments(element);

  //Zero angle bend only needs one element.
  std::string thename = element->name + "_1_of_1";

  // Single element for zero bend angle or dontSplitSBends=1, therefore nSBends = 1
  if ((!BDS::IsFinite(element->angle)) || (nSBends == 1))
    {
      std::pair<G4ThreeVector,G4ThreeVector> faces = BDS::CalculateFaces(-angleIn, -angleOut);
      BDSBeamPipeInfo* bpInfo = PrepareBeamPipeInfo(element, faces.first, faces.second);
      return (new BDSSectorBend(thename,
				length,
				element->angle,
				bField,
				bPrime,
				bpInfo,
				PrepareMagnetOuterInfo(element,-angleIn,-angleOut)));
    }
  else if ((!BDS::IsFinite(element->e1) && !BDS::IsFinite(element->e2)) && (nSBends > 1))
    {
      BDSLine* sbendline  = new BDSLine(element->name);
      if (BDSAcceleratorComponentRegistry::Instance()->IsRegistered(element->name))
	{
	  BDSAcceleratorComponent* sBendComponent =  BDSAcceleratorComponentRegistry::Instance()->GetComponent(element->name);
	  
	  for (G4int n = 0; n < nSBends; n++)
	    {sbendline->AddComponent(sBendComponent);}
	}
      else {

      G4double semiangle = element->angle / (G4double) nSBends;
      G4double semilength = length / (G4double) nSBends;
      G4double angle = -semiangle*0.5;

      std::pair<G4ThreeVector,G4ThreeVector> faces = BDS::CalculateFaces(angle, angle);
      BDSBeamPipeInfo* bpInfo = PrepareBeamPipeInfo(element, faces.first, faces.second);
      BDSMagnetOuterInfo *magnetOuterInfo = PrepareMagnetOuterInfo(element, angle, angle);

      BDSSectorBend *newSBendComponent = new BDSSectorBend(thename,
                                                        semilength,
                                                        semiangle,
                                                        bField,
                                                        bPrime,
							   bpInfo,
                                                        magnetOuterInfo);

      newSBendComponent->SetBiasVacuumList(element->biasVacuumList);
      newSBendComponent->SetBiasMaterialList(element->biasMaterialList);
      newSBendComponent->SetPrecisionRegion(element->precisionRegion);
      newSBendComponent->Initialise();

      // register component and memory
      BDSAcceleratorComponentRegistry::Instance()->RegisterComponent(newSBendComponent,false);

      for (G4int n = 0; n < nSBends; n++)
      {
        sbendline->AddComponent(newSBendComponent);
      }
    }
    return sbendline;
  }


  else  //Otherwise, create line of sbend segments
  {
    BDSLine* sbendline = CreateSBendLine(element, nSBends, bField, bPrime);
    return sbendline;
  }
}

BDSLine* BDSComponentFactory::CreateSBendLine(Element const* element,
                            G4int nSBends,
                            G4double bField,
                            G4double bPrime)
{
  BDSLine* sbendline = new BDSLine(element->name);

  G4double length = element->l*CLHEP::m;
  // prepare one name for all that makes sense
  std::string thename = element->name + "_1_of_" + std::to_string(nSBends);
  //calculate their angles and length
  G4double semiangle  = element->angle / (G4double) nSBends;
  G4double semilength = length / (G4double) nSBends;
  G4double angleIn    = element->e1*CLHEP::rad;
  G4double angleOut   = element->e2*CLHEP::rad;

  BDSMagnetOuterInfo* magnetOuterInfoCheck = PrepareMagnetOuterInfo(element,angleIn,angleOut);
  CheckBendLengthAngleWidthCombo(semilength, semiangle, magnetOuterInfoCheck->outerDiameter, thename);
  // clean up
  delete magnetOuterInfoCheck;

  G4double deltastart = -element->e1/(0.5*(nSBends-1));
  G4double deltaend   = -element->e2/(0.5*(nSBends-1));

  for (int i = 0; i < nSBends; ++i)
    {
      thename = element->name + "_"+std::to_string(i+1)+"_of_" + std::to_string(nSBends);

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
      // Check for intersection of angled faces.
      G4double intersectionX = BDS::CalculateFacesOverlapRadius(angleIn,angleOut,semilength);
      BDSMagnetOuterInfo* magnetOuterInfo = PrepareMagnetOuterInfo(element,angleIn,angleOut);

      // Every geometry type has a completely arbitrary factor of 1.25 except cylindrical
      G4double magnetRadius= 0.625*magnetOuterInfo->outerDiameter*CLHEP::mm;

      if (magnetOuterInfo->geometryType == BDSMagnetGeometryType::cylindrical){
        magnetRadius= 0.5*magnetOuterInfo->outerDiameter*CLHEP::mm;}

      std::pair<G4ThreeVector,G4ThreeVector> faces = BDS::CalculateFaces(angleIn, angleOut);
      BDSBeamPipeInfo* bpInfo = PrepareBeamPipeInfo(element, faces.first, faces.second);
      
      //Check if intersection is within radius
      if ((BDS::IsFinite(intersectionX)) && (std::abs(intersectionX) < magnetRadius))
        {
          G4cerr << __METHOD_NAME__ << "Angled faces of element "<< thename << " intersect within the magnet radius." << G4endl;
          exit(1);
        }

      BDSSectorBend* oneBend = new BDSSectorBend(thename,
                             semilength,
                             semiangle,
                             bField,
                             bPrime,
						 bpInfo,
                             magnetOuterInfo);

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
  
  return sbendline;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateRBend(G4double angleIn,
                                    G4double angleOut)
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  PoleFaceRotationsNotTooLarge(element);
  
  // require drift next to non-zero poleface or rbend with matching poleface
  if (BDS::IsFinite(element->e1) )
    {
      if (
	  prevElement &&
	  !(prevElement->type == ElementType::_DRIFT) &&
	  !(prevElement->type == ElementType::_RBEND && !BDS::IsFinite(prevElement->e2 + element->e1) )
	  )
	{
	  G4cerr << __METHOD_NAME__ << "RBend with non-zero incoming poleface requires previous element to be a Drift or RBend with opposite outcoming poleface" << G4endl;
	  exit(1);
	}
    }

  if (BDS::IsFinite(element->e2) )
    {
      if (
	  nextElement &&
	  !(nextElement->type == ElementType::_DRIFT) &&
	  !(nextElement->type == ElementType::_RBEND && !BDS::IsFinite(nextElement->e1 + element->e2) )
	  )
	{
	  G4cerr << __METHOD_NAME__ << "RBend with non-zero incoming poleface requires next element to be a Drift or RBend with opposite incoming poleface" << G4endl;
	  exit(1);
	}
    }

  // calculate length of central straight length and edge sections
  // unfortunately, this has to be duplicated here as we need to
  // calculated the magnetic field length (less than the full length)
  // in case we need to calculate the field
  G4double outerRadius = PrepareOuterDiameter(element)*0.5;
  G4double angle       = element->angle;
  G4double chordLength = element->l*CLHEP::m;
  //G4double straightSectionChord = outerRadius / (tan(0.5*fabs(angle)) + tan((0.5*CLHEP::pi) - (0.5*fabs(angle))) );
  G4double magFieldLength = chordLength;

  CheckBendLengthAngleWidthCombo(chordLength, angle, 2*outerRadius, element->name);

  // magnetic field
  // CHECK SIGNS OF B, B', ANGLE
  G4double bField;
  if(BDS::IsFinite(element->B))
    {
      // angle = arc length/radius of curvature = L/rho = (B*L)/(B*rho)
      bField = element->B * CLHEP::tesla;
      G4double rho = brho/bField;
      //element->angle  = - bField * length / brho;
      element->angle  = - 2.0*asin(magFieldLength/2.0/rho);
#ifdef BDSDEBUG
      G4cout << "calculated angle from field - now " << element->angle << G4endl;
#endif
    }
  else
    {
      element->angle *= -1;
      // arc length = radius*angle
      //            = (geometrical length/(2.0*sin(angle/2))*angle
      G4double arclength;
      if (BDS::IsFinite(element->angle))
	{arclength = 0.5*magFieldLength * std::abs(element->angle) / sin(std::abs(element->angle)*0.5);}
      else
	{arclength = magFieldLength;}
      // B = Brho/rho = Brho/(arc length/angle)
      // charge in e units
      // multiply once more with ffact to not flip fields in bends
      bField = - brho * element->angle / arclength * charge * BDSGlobalConstants::Instance()->FFact();
      element->B = bField/CLHEP::tesla;
#ifdef BDSDEBUG
      G4cout << "calculated field from angle - angle,field = " << element->angle << " " << element->B << G4endl;
#endif
    }
  
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  // Brho is already in G4 units, but k1 is not -> multiply k1 by m^-2
  G4double bPrime = - brho * (element->k1 / CLHEP::m2);

  std::pair<G4ThreeVector,G4ThreeVector> faces = BDS::CalculateFaces(angleIn, angleOut);
  BDSBeamPipeInfo* bpInfo = PrepareBeamPipeInfo(element, faces.first, faces.second);
  
  return (new BDSRBend( element->name,
			element->l*CLHEP::m,
			bField,
			bPrime,
			element->angle,
			bpInfo,
			PrepareMagnetOuterInfo(element,angleIn,angleOut)));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateHKick()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}  
  
  G4double length = element->l*CLHEP::m;
  
  G4double bField;
  if(BDS::IsFinite(element->B))
    {
      // angle = arc length/radius of curvature = L/rho = (B*L)/(B*rho)
      bField = element->B * CLHEP::tesla;
      element->angle  = -bField * length / brho;
    }
  else
    {
      // B = Brho/rho = Brho/(arc length/angle)
      // charge in e units
      // multiply once more with ffact to not flip fields in kicks defined with angle
      bField = - brho * element->angle / length * charge * BDSGlobalConstants::Instance()->FFact(); // charge in e units
      element->B = bField/CLHEP::tesla;
    }
  
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  // Brho is already in G4 units, but k1 is not -> multiply k1 by m^-2
  //G4double bPrime = - _brho * (element->k1 / CLHEP::m2);
  
  return (new BDSKicker(element->name,
			element->l * CLHEP::m,
			bField,
			element->angle,
			BDSMagnetType::hkicker,
			PrepareBeamPipeInfo(element),
			PrepareMagnetOuterInfo(element)));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateVKick()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}
  
  G4double length = element->l*CLHEP::m;
  
  // magnetic field
  G4double bField;
  if(BDS::IsFinite(element->B))
    {
      // angle = arc length/radius of curvature = L/rho = (B*L)/(B*rho)
      bField = element->B * CLHEP::tesla;
      element->angle  = -bField * length / brho;
    }
  else
    {
      // B = Brho/rho = Brho/(arc length/angle)
      // charge in e units
      // multiply once more with ffact to not flip fields in kicks
      bField = - brho * element->angle / length * charge * BDSGlobalConstants::Instance()->FFact();
      element->B = bField/CLHEP::tesla;
    }
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  // Brho is already in G4 units, but k1 is not -> multiply k1 by m^-2
  //G4double bPrime = - _brho * (element->k1 / CLHEP::m2);
  
  return (new BDSKicker(element->name,
			element->l * CLHEP::m,
			bField,
			element->angle,
			BDSMagnetType::vkicker,
			PrepareBeamPipeInfo(element),
			PrepareMagnetOuterInfo(element)
			));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateQuad()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}
  
  // magnetic field
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  // Brho is already in G4 units, but k1 is not -> multiply k1 by m^-2
  G4double bPrime = - brho * (element->k1 / CLHEP::m2);

  return (new BDSQuadrupole( element->name,
			     element->l * CLHEP::m,
			     bPrime,
			     PrepareBeamPipeInfo(element),
			     PrepareMagnetOuterInfo(element)));
}  
  
BDSAcceleratorComponent* BDSComponentFactory::CreateSextupole()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}
  
  // magnetic field 
  // B'' = d^2By/dx^2 = Brho * (1/Brho d^2By/dx^2) = Brho * k2
  // brho is in Geant4 units, but k2 is not -> multiply k2 by m^-3
  G4double bDoublePrime = - brho * (element->k2 / CLHEP::m3);
  
#ifdef BDSDEBUG 
  G4cout << "---->creating Sextupole,"
	 << " name= " << element->name
	 << " l= " << element->l << "m"
	 << " k2= " << element->k2 << "m^-3"
	 << " brho= " << std::abs(brho)/(CLHEP::tesla*CLHEP::m) << "T*m"
	 << " B''= " << bDoublePrime/(CLHEP::tesla/CLHEP::m2) << "T/m^2"
	 << " material= " << element->outerMaterial
	 << G4endl;
#endif
  
  return (new BDSSextupole( element->name,
			    element->l * CLHEP::m,
			    bDoublePrime,
			    PrepareBeamPipeInfo(element),
			    PrepareMagnetOuterInfo(element)));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateOctupole()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}
  
  // magnetic field  
  // B''' = d^3By/dx^3 = Brho * (1/Brho d^3By/dx^3) = Brho * k3
  // brho is in Geant4 units, but k3 is not -> multiply k3 by m^-4
  G4double bTriplePrime = - brho * (element->k3 / (CLHEP::m3*CLHEP::m));
  
#ifdef BDSDEBUG 
  G4cout << "---->creating Octupole,"
	 << " name= " << element->name
	 << " l= " << element->l << "m"
	 << " k3= " << element->k3 << "m^-4"
	 << " brho= " << std::abs(brho)/(CLHEP::tesla*CLHEP::m) << "T*m"
	 << " B'''= " << bTriplePrime/(CLHEP::tesla/CLHEP::m3) << "T/m^3"
	 << " material= " << element->outerMaterial
	 << G4endl;
#endif
  
  return ( new BDSOctupole( element->name,
			    element->l * CLHEP::m,
			    bTriplePrime,
			    PrepareBeamPipeInfo(element),
			    PrepareMagnetOuterInfo(element)));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateDecapole()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}
  
  // magnetic field  
  // B''' = d^4By/dx^4 = Brho * (1/Brho d^4By/dx^4) = Brho * k4
  // brho is in Geant4 units, but k4 is not -> multiply k4 by m^-5
  G4double bQuadruplePrime = - brho * (element->k4 / (CLHEP::m3*CLHEP::m2));
  
#ifdef BDSDEBUG 
  G4cout << "---->creating Decapole,"
	 << " name= " << element->name
	 << " l= " << element->l << "m"
	 << " k4= " << element->k4 << "m^-5"
	 << " brho= " << std::abs(brho)/(CLHEP::tesla*CLHEP::m) << "T*m"
	 << " B''''= " << bQuadruplePrime/(CLHEP::tesla/CLHEP::m3*CLHEP::m) << "T/m^4"
	 << " material= " << element->outerMaterial
	 << G4endl;
#endif
  
  return ( new BDSDecapole( element->name,
			    element->l * CLHEP::m,
			    bQuadruplePrime,
			    PrepareBeamPipeInfo(element),
			    PrepareMagnetOuterInfo(element)));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateMultipole()
{
 if(!HasSufficientMinimumLength(element))
    {return nullptr;}
 
#ifdef BDSDEBUG 
  G4cout << "---->creating Multipole,"
	 << " name= " << element->name
	 << " l= " << element->l << "m"
	 << " material= " << element->outerMaterial
	 << G4endl;
#endif
  // magnetic field
  std::list<double>::iterator kit;
  
#ifdef BDSDEBUG 
  G4cout << " knl={ ";
#endif
  for(kit=(element->knl).begin();kit!=(element->knl).end();kit++)
    {
#ifdef BDSDEBUG 
      G4cout<<(*kit)<<", ";
#endif
      (*kit) /= element->l; 
    }
#ifdef BDSDEBUG 
  G4cout << "}";
#endif
  
#ifdef BDSDEBUG 
  G4cout << " ksl={ ";
#endif
  for(kit=(element->ksl).begin();kit!=(element->ksl).end();kit++)
    {
#ifdef BDSDEBUG 
      G4cout<<(*kit)<<" ";
#endif
      (*kit) /= element->l; 
    }
#ifdef BDSDEBUG 
  G4cout << "}" << G4endl;
#endif

  return (new BDSMultipole( element->name,
			    element->l * CLHEP::m,
			    element->knl,
			    element->ksl,
			    PrepareBeamPipeInfo(element),
			    PrepareMagnetOuterInfo(element)));
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
  
#ifdef BDSDEBUG 
  G4cout << "---->creating Element,"
	 << " name = " << element->name
	 << " l = " << element->l << "m"
	 << " outerDiameter = "  << element->outerDiameter << "m"
	 << " bmapZOffset = "	 << element->bmapZOffset * CLHEP::m << "mm"
	 << " precision region " << element->precisionRegion
	 << G4endl;
#endif

  return (new BDSElement(element->name,
			 element->l * CLHEP::m,
			 element->outerDiameter * CLHEP::m,
			 element->geometryFile,
			 element->bmapFile,
			 element->bmapZOffset * CLHEP::m));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateSolenoid()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}
  
  // B = B/Brho * Brho = ks * Brho
  // brho is in Geant4 units, but ks is not -> multiply ks by m^-1
  G4double bField;
  if(BDS::IsFinite(element->B))
    {
      bField = element->B * CLHEP::tesla;
      element->ks = (bField/brho) / CLHEP::m;
    }
  else
    {
      bField = (element->ks/CLHEP::m) * brho;
      element->B = bField/CLHEP::tesla;
    }
  
#ifdef BDSDEBUG 
  G4cout << "---->creating Solenoid,"
	 << " name = " << element->name
	 << " l = " << element->l << " m,"
	 << " ks = " << element->ks << " m^-1,"
	 << " brho = " << std::abs(brho)/(CLHEP::tesla*CLHEP::m) << " T*m,"
	 << " B = " << bField/CLHEP::tesla << " T,"
	 << " material = \"" << element->outerMaterial << "\""
	 << G4endl;
#endif
  
  return (new BDSSolenoid( element->name,
			   element->l * CLHEP::m,
			   bField,
			   PrepareBeamPipeInfo(element),
			   PrepareMagnetOuterInfo(element)));
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
  
#ifdef BDSDEBUG 
  G4cout << "---->creating muspoiler,"
	 << " name = " << element->name 
	 << " length = " << element->l
	 << " B = " << element->B*CLHEP::tesla << " m*T"
	 << G4endl;
#endif
  
  return (new BDSMuSpoiler(element->name,
			   element->l*CLHEP::m,
			   element->B * CLHEP::tesla,
			   PrepareBeamPipeInfo(element),
			   PrepareMagnetOuterInfo(element)));
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
  
  return (new BDSTerminator("terminator", 
			    length));
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
							  const G4ThreeVector inputFaceNormal,
							  const G4ThreeVector outputFaceNormal) const
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
					      inputFaceNormal,
					      outputFaceNormal);
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
  G4double tilt    = element->tilt    * CLHEP::rad;

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
				    model.frequency, // TBC - units
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
