#include "BDSComponentFactory.hh"

// elements
#include "BDSAwakeScintillatorScreen.hh"
#include "BDSCollimatorElliptical.hh"
#include "BDSCollimatorRectangular.hh"
#include "BDSDegrader.hh"
#include "BDSDrift.hh"
#include "BDSDump.hh"
#include "BDSElement.hh"
#include "BDSKicker.hh"
#include "BDSLaserWire.hh"
#include "BDSLine.hh"
#include "BDSMuSpoiler.hh"
#include "BDSOctupole.hh"
#include "BDSDecapole.hh"
#include "BDSQuadrupole.hh"
#include "BDSRBend.hh"
#include "BDSRfCavity.hh"
#include "BDSSampler.hh"
#include "BDSSamplerCylinder.hh"
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
#include "BDSBeamline.hh"
#include "BDSBeamPipeType.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetType.hh"
#include "BDSMagnetGeometryType.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4GeometryTolerance.hh"

#include "parser/elementtype.h"

#include <cmath>
#include <string>

using namespace GMAD;

BDSComponentFactory::BDSComponentFactory()
{
  lengthSafety = BDSGlobalConstants::Instance()->GetLengthSafety();
  //
  // compute magnetic rigidity brho
  // formula: B(Tesla)*rho(m) = p(GeV)/(0.299792458 * |charge(e)|)
  //
  // charge (in e units)
  charge = BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGCharge();
  // momentum (in GeV/c)

  G4double momentum = BDSGlobalConstants::Instance()->GetBeamMomentum()/CLHEP::GeV;
  // rigidity (in T*m)
  brho = BDSGlobalConstants::Instance()->GetFFact()*( momentum / 0.299792458);
  
  // rigidity (in Geant4 units)
  brho *= (CLHEP::tesla*CLHEP::m);

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Rigidity (Brho) : "<< fabs(brho)/(CLHEP::tesla*CLHEP::m) << " T*m"<<G4endl;
#endif
}

BDSComponentFactory::~BDSComponentFactory()
{;}

BDSAcceleratorComponent* BDSComponentFactory::CreateComponent(Element* elementIn, Element* prevElementIn, Element* nextElementIn)
{
  element = elementIn;
  prevElement = prevElementIn;
  nextElement = nextElementIn;

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "named: \"" << element->name << "\"" << G4endl;  
#endif
  // check if the component already exists and return that
  // don't use registry for output elements since reliant on unique name
  if (element->type != ElementType::_SAMPLER &&
      element->type != ElementType::_CSAMPLER &&
      element->type != ElementType::_DUMP &&
      BDSAcceleratorComponentRegistry::Instance()->IsRegistered(element->name))
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
  case ElementType::_SAMPLER:
    component = CreateSampler(); break;
  case ElementType::_DRIFT:
    component = CreateDrift(); break;
  case ElementType::_RF:
    component = CreateRF(); break;
  case ElementType::_SBEND:
    component = CreateSBend(); break;
  case ElementType::_RBEND:
    component = CreateRBend(); break;
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
  case ElementType::_CSAMPLER:
    component = CreateCSampler(); break; 
  case ElementType::_DUMP:
    component = CreateDump(); break; 
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
  case ElementType::_MATERIAL:
  case ElementType::_ATOM:
    component = nullptr;
    break;
  default:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory: type: " << element->type << G4endl; 
#endif
    G4Exception("Error: BDSComponentFactory: type not found.", "-1", FatalErrorInArgument, "");   
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
      BDSAcceleratorComponentRegistry::Instance()->RegisterComponent(component);
    }
  
  return component;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateSampler()
{
  return (new BDSSampler(element->name));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateCSampler()
{
  if( element->l < 1.E-4 )
    {element->l = 1.0;}
  return (new BDSSamplerCylinder( element->name,
				  element->l * CLHEP::m,
				  element->r * CLHEP::m ));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateDump()
{
  return (new BDSDump( element->name,
		       BDSGlobalConstants::Instance()->GetSamplerLength()));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateTeleporter()
{
  // This relies on things being added to the beamline immediately
  // after they've been created
  G4double teleporterLength = BDSGlobalConstants::Instance()->GetTeleporterLength() - 1e-8;

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

BDSAcceleratorComponent* BDSComponentFactory::CreateDrift()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

#ifdef BDSDEBUG
  G4cout << "---->creating Drift,"
	 << " name= " << element->name
	 << " l= " << element->l << "m"
	 << G4endl;
#endif

  // Match poleface from previous and next element
  double e1 = (prevElement) ? ( prevElement->e2 * CLHEP::rad ) : 0.0;
  double e2 = (nextElement) ? ( nextElement->e1 * CLHEP::rad ) : 0.0;
  
  return (new BDSDrift( element->name,
			element->l*CLHEP::m,
			e1,
			e2,
			PrepareBeamPipeInfo(element) ));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateRF()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}
  
  return (new BDSRfCavity( element->name,
			   element->l * CLHEP::m,
			   element->gradient,
			   PrepareBeamPipeInfo(element),
			   PrepareMagnetOuterInfo(element)));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateSBend()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  // check if poleface is not too large
  if (fabs(element->e1) > (0.5*CLHEP::halfpi))
    {
      G4cerr << __METHOD_NAME__ << "Poleface angle e1 " << element->e1 << " is greater than pi/4" << G4endl;
      exit(1);
    }
  if (fabs(element->e2) > (0.5*CLHEP::halfpi))
    {
      G4cerr << __METHOD_NAME__ << "Poleface angle e2 " << element->e2 << " is greater than pi/4" << G4endl;
      exit(1);
    }

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
  G4double magFieldLength = length;
  
  // magnetic field
  // MAD conventions:
  // 1) a positive bend angle represents a bend to the right, i.e.
  // towards negative x values (even for negative charges??)
  // 2) a positive K1 = 1/|Brho| dBy/dx means horizontal focusing of
  // positive charges
  // CHECK SIGNS 
  
  G4double bField;
  if(element->B != 0)
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
    {
      element->angle *= -1;
      //    bField = - 2 * brho * sin(element->angle/2.0) / magFieldLength;
      // charge in e units
      // multiply once more with ffact to not flip fields in bends
      bField = - brho * element->angle/magFieldLength * charge * BDSGlobalConstants::Instance()->GetFFact();
      element->B = bField/CLHEP::tesla;
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "B calculated from angle (" << element->angle << ") : " << bField << G4endl;
#endif
    }
  
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  // Brho is already in G4 units, but k1 is not -> multiply k1 by m^-2
  G4double bPrime = - brho * (element->k1 / CLHEP::m2);

  //calculate number of sbends to split parent into
  //if maximum distance between arc path and straight path larger than 1mm, split sbend into N chunks,
  //this also works when maximum distance is less than 1mm as there will just be 1 chunk!
  double aperturePrecision = 1.0; // in mm
  // from formula: L/2 / N tan (angle/N) < precision. (L=physical length)
  // add poleface rotations onto angle as absolute number (just to be safe)
  double totalAngle = std::abs(element->angle) + std::abs(element->e1) + std::abs(element->e2);
  int nSbends = (int) ceil(std::sqrt(length*totalAngle/2/aperturePrecision));
  if (nSbends==0) nSbends = 1; // can happen in case angle = 0
  if (BDSGlobalConstants::Instance()->DontSplitSBends())
    {nSbends = 1;}  // use for debugging
  if (nSbends % 2 == 0)
    {nSbends += 1;} // always have odd number of poles for poleface rotations
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " splitting sbend into " << nSbends << " sbends" << G4endl;
#endif
  // prepare one name for all that makes sense
  std::string thename = element->name + "_1_of_" + std::to_string(nSbends);
  //calculate their angle and length
  double semiangle  = element->angle / (double) nSbends;
  double semilength = length / (double) nSbends;

  //create Line to put them in
  BDSLine* sbendline = new BDSLine(element->name);
  //create sbends and put them in the line
  BDSBeamPipeInfo*    beamPipeInfo    = PrepareBeamPipeInfo(element);
  BDSMagnetOuterInfo* magnetOuterInfo = PrepareMagnetOuterInfo(element);

  CheckBendLengthAngleWidthCombo(semilength, semiangle, magnetOuterInfo->outerDiameter, thename);
  
  G4double deltastart = 0;
  G4double deltaend   = 0;
  G4double anglein    = 0;
  G4double angleout   = 0;

  // prepare one sbend segment
  // create a line of this sbend repeatedly

  for (int i = 0; i < nSbends; ++i)
    {
      //Calculate change in angle up to middle wedge
      if (element->e1 != 0)
        {deltastart = -element->e1/(0.5*(nSbends-1));}
      if (element->e2 != 0)
        {deltaend = -element->e2/(0.5*(nSbends-1));}
      
      //Central wedge as before, poleface angle(s) added/subtracted either side as appropriate.
      if (i == 0.5*(nSbends-1))
        {
          anglein = -0.5*element->angle/(nSbends);
          angleout = -0.5*element->angle/(nSbends);
        }
      else if (i < 0.5*(nSbends-1))
        {
          anglein = -0.5*element->angle/(nSbends) - element->e1 - (i*deltastart);
          angleout = -0.5*element->angle/nSbends - ((0.5*(nSbends-3)-i)*deltastart);
        }
      else if (i > 0.5*(nSbends-1))
        {
          anglein  = -0.5*element->angle/nSbends + ((0.5*(nSbends+1)-i)*deltaend);
          angleout = -0.5*element->angle/nSbends + (i-(0.5*(nSbends-1)))*deltaend;
        }

      thename = element->name + "_"+std::to_string(i+1)+"_of_" + std::to_string(nSbends);
      
      BDSBeamPipeInfo*    localBeamPipeInfo    = new BDSBeamPipeInfo(*beamPipeInfo);
      BDSMagnetOuterInfo* localMagnetOuterInfo = new BDSMagnetOuterInfo(*magnetOuterInfo);
      BDSSectorBend* oneBend = new BDSSectorBend(thename,
						 semilength,
						 semiangle,
						 bField,
						 bPrime,
						 anglein,
						 angleout,
						 localBeamPipeInfo,
						 localMagnetOuterInfo);
      
      oneBend->SetBiasVacuumList(element->biasVacuumList);
      oneBend->SetBiasMaterialList(element->biasMaterialList);
      
      sbendline->AddComponent(oneBend);
    }

  // clean up
  delete beamPipeInfo;
  delete magnetOuterInfo;
  
  return sbendline;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateRBend()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}
    
  if (fabs(element->e1) > (0.5*CLHEP::halfpi))
    {
      G4cerr << __METHOD_NAME__ << "Poleface angle e1 " << element->e1 << " is greater than pi/4" << G4endl;
      exit(1);
    }
  if (fabs(element->e2) > (0.5*CLHEP::halfpi))
    {
      G4cerr << __METHOD_NAME__ << "Poleface angle e2 " << element->e2 << " is greater than pi/4" << G4endl;
      exit(1);
    }

  // require drift next to non-zero poleface or rbend with matching poleface
  if (BDS::IsFinite(element->e1) )
    {
      if (
	  prevElement &&
	  !(prevElement->type == ElementType::_DRIFT) &&
	  !(prevElement->type == ElementType::_RBEND && !BDS::IsFinite(prevElement->e2 + element->e1) )
	  )
	{
	  G4cerr << __METHOD_NAME__ << "SBend with non-zero incoming poleface requires previous element to be a Drift or RBend with opposite outcoming poleface" << G4endl;
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
	  G4cerr << __METHOD_NAME__ << "SBend with non-zero incoming poleface requires next element to be a Drift or RBend with opposite incoming poleface" << G4endl;
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
  G4double straightSectionChord = outerRadius / (tan(0.5*fabs(angle)) + tan((0.5*CLHEP::pi) - (0.5*fabs(angle))) );
  G4double magFieldLength = chordLength - (2.0*straightSectionChord);

  CheckBendLengthAngleWidthCombo(chordLength, angle, 2*outerRadius, element->name);

  // magnetic field
  // CHECK SIGNS OF B, B', ANGLE
  G4double bField;
  if(element->B != 0)
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
	{arclength = 0.5*magFieldLength * fabs(element->angle) / sin(fabs(element->angle)*0.5);}
      else
	{arclength = magFieldLength;}
      // B = Brho/rho = Brho/(arc length/angle)
      // charge in e units
      // multiply once more with ffact to not flip fields in bends
      bField = - brho * element->angle / arclength * charge * BDSGlobalConstants::Instance()->GetFFact();
      element->B = bField/CLHEP::tesla;
#ifdef BDSDEBUG
      G4cout << "calculated field from angle - angle,field = " << element->angle << " " << element->B << G4endl;
#endif
    }
  
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  // Brho is already in G4 units, but k1 is not -> multiply k1 by m^-2
  G4double bPrime = - brho * (element->k1 / CLHEP::m2);
  
  G4double anglein    = element->e1*CLHEP::rad;
  G4double angleout   = element->e2*CLHEP::rad;
  
  return (new BDSRBend( element->name,
			element->l*CLHEP::m,
			bField,
			bPrime,
			element->angle,
			anglein,
			angleout,
			PrepareBeamPipeInfo(element),
			PrepareMagnetOuterInfo(element)));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateHKick()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}  
  
  G4double length = element->l*CLHEP::m;
  
  // magnetic field
  G4double bField;
  if(element->B != 0)
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
      bField = - brho * element->angle / length * charge * BDSGlobalConstants::Instance()->GetFFact(); // charge in e units
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
  if(element->B != 0)
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
      bField = - brho * element->angle / length * charge * BDSGlobalConstants::Instance()->GetFFact();
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
	 << " brho= " << fabs(brho)/(CLHEP::tesla*CLHEP::m) << "T*m"
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
	 << " brho= " << fabs(brho)/(CLHEP::tesla*CLHEP::m) << "T*m"
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
	 << " brho= " << fabs(brho)/(CLHEP::tesla*CLHEP::m) << "T*m"
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
			 element->bmapZOffset * CLHEP::m
			  ));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateSolenoid()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}
  
  // magnetic field
  //
  // B = B/Brho * Brho = ks * Brho
  // brho is in Geant4 units, but ks is not -> multiply ks by m^-1
  G4double bField;
  if(element->B != 0)
    {
      bField = element->B * CLHEP::tesla;
      element->ks  = (bField/brho) / CLHEP::m;
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
	 << " brho = " << fabs(brho)/(CLHEP::tesla*CLHEP::m) << " T*m,"
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
				      element->material);
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
                     element->material);
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
    {
        degraderOffset = element->degraderOffset*CLHEP::m;
    }
    
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

	
  G4ThreeVector direction = 
    G4ThreeVector(element->xdir,element->ydir,element->zdir);
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
	return (new BDSScintillatorScreen( element->name, element->tscint*CLHEP::m, (element->angle-0.78539816339)*CLHEP::rad, "ups923a",BDSGlobalConstants::Instance()->GetVacuumMaterial())); //Name, scintillator thickness, angle in radians (relative to -45 degrees)
}


BDSAcceleratorComponent* BDSComponentFactory::CreateAwakeScreen(){
	
#ifdef BDSDEBUG 
        G4cout << "---->creating Awake Screen,"
	       << "twindow = " << element->twindow*1e3/CLHEP::um << " um"
	       << "tscint = " << element->tscint*1e3/CLHEP::um << " um"
	       << "windowmaterial = " << element->windowmaterial << " um"
	       << "scintmaterial = " << element->scintmaterial << " um"
               << G4endl;
#endif
	return (new BDSAwakeScintillatorScreen(element->name, element->scintmaterial, element->tscint*1e3, element->angle, element->twindow*1e3, element->windowmaterial)); //Name
}

BDSAcceleratorComponent* BDSComponentFactory::CreateTransform3D(){
	
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
  G4double length = BDSGlobalConstants::Instance()->GetSamplerLength();
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

BDSMagnetOuterInfo* BDSComponentFactory::PrepareMagnetOuterInfo(Element* element)
{
  BDSMagnetOuterInfo* info = new BDSMagnetOuterInfo();
  // magnet geometry type
  if (element->magnetGeometryType == "")
    info->geometryType = BDSGlobalConstants::Instance()->GetMagnetGeometryType();
  else
    info->geometryType = BDS::DetermineMagnetGeometryType(element->magnetGeometryType);

  // outer diameter
  G4double outerDiameter = element->outerDiameter*CLHEP::m;
  if (outerDiameter < 1e-6)
    {//outerDiameter not set - use global option as default
      outerDiameter = BDSGlobalConstants::Instance()->GetOuterDiameter();
    }
  info->outerDiameter = outerDiameter;

  // outer material
  G4Material* outerMaterial;
  if(element->outerMaterial == "")
    {
      G4String defaultMaterialName = BDSGlobalConstants::Instance()->GetOuterMaterialName();
      outerMaterial = BDSMaterials::Instance()->GetMaterial(defaultMaterialName);
    }
  else
    {outerMaterial = BDSMaterials::Instance()->GetMaterial(element->outerMaterial);}
  info->outerMaterial = outerMaterial;
  
  return info;
}

G4double BDSComponentFactory::PrepareOuterDiameter(Element* element)
{
  G4double outerDiameter = element->outerDiameter*CLHEP::m;
  if (outerDiameter < 1e-6)
    {//outerDiameter not set - use global option as default
      outerDiameter = BDSGlobalConstants::Instance()->GetOuterDiameter();
    }
  // returns in metres
  return outerDiameter;
}

BDSBeamPipeInfo* BDSComponentFactory::PrepareBeamPipeInfo(Element* element)
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
					      element->beampipeMaterial);  
  return info;
}

BDSTiltOffset* BDSComponentFactory::CreateTiltOffset(Element* element)
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
