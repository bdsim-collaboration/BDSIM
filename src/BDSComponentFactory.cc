
#include "BDSComponentFactory.hh"

// elements
#ifdef USE_AWAKE
#include "BDSAwakeScintillatorScreen.hh"
#include "BDSAwakeSpectrometer.hh"
#endif
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
#include "BDSScreen.hh"
#include "BDSTerminator.hh"
#include "BDSTeleporter.hh"
#include "BDSTiltOffset.hh"
#include "BDSTransform3D.hh"
#include "BDSBendBuilder.hh"

// general
#include "BDSAcceleratorComponentRegistry.hh"
#include "BDSBeamPipeType.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSCavityInfo.hh"
#include "BDSCavityType.hh"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldFactory.hh"
#include "BDSFieldType.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIntegratorSet.hh"
#include "BDSIntegratorType.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetGeometryType.hh"
#include "BDSMagnetStrength.hh"
#include "BDSMagnetType.hh"
#include "BDSMaterials.hh"
#include "BDSParser.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4GeometryTolerance.hh"
#include "G4ParticleDefinition.hh"
#include "G4Transform3D.hh"

#include "parser/elementtype.h"
#include "parser/cavitymodel.h"

#include <cmath>
#include <string>
#include <utility>
using namespace GMAD;


BDSComponentFactory::BDSComponentFactory()
{
  lengthSafety  = BDSGlobalConstants::Instance()->LengthSafety();
  charge        = BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGCharge();
  brho          = BDSGlobalConstants::Instance()->BRho();
  integratorSet = BDS::IntegratorSet(BDSGlobalConstants::Instance()->IntegratorSet());

  // prepare rf cavity model info from parser
  PrepareCavityModels();
  
  notSplit          = BDSGlobalConstants::Instance()->DontSplitSBends();
  includeFringe     = BDSGlobalConstants::Instance()->IncludeFringeFields();
  thinElementLength = BDSGlobalConstants::Instance()->ThinElementLength();
}

BDSComponentFactory::~BDSComponentFactory()
{
  for(auto info : cavityInfos)
    {delete info.second;}
}

BDSAcceleratorComponent* BDSComponentFactory::CreateComponent(Element* elementIn,
                                       const std::vector<GMAD::Element*>& prevElements,
                                       const std::vector<GMAD::Element*>& nextElements)
{
  element      = elementIn;

  G4double angleIn  = 0.0;
  G4double angleOut = 0.0;
  G4bool registered = false;
  // Used for multiple instances of the same element but different poleface rotations.
  // Ie only a drift is modified to match the pole face rotation of a magnet.
  G4bool willModify = false;

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "named: \"" << element->name << "\"" << G4endl;  
#endif

  // set next/previous element to be last (and only non thinmultipole) element in the vector
  if (prevElements.empty())
    {prevElement = nullptr;}
  else
    {prevElement = prevElements.back();}
  if (nextElements.empty())
    {nextElement = nullptr;}
  else
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
	{angleIn += -0.5*(prevElement->angle);} // won't work if only field set TBC

      if (nextElement && (nextElement->type == ElementType::_RBEND))
	{angleOut += 0.5*nextElement->angle;} // won't work if only field set TBC

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
  else if (element->type == ElementType::_THINMULT)
    {
      if (nextElement && (BDS::IsFinite(nextElement->e1)))
      {
        angleIn += nextElement->e1 * CLHEP::rad;
        willModify  = true;
      }
      else if (prevElement && (BDS::IsFinite(prevElement->e2)))
      {
        angleIn -= prevElement->e2 * CLHEP::rad;
        willModify  = true;
      }
      if (nextElement && (nextElement->type == ElementType::_RBEND))
      {
        willModify = true;
        angleIn += 0.5*nextElement->angle;
      }
      if (prevElement && (prevElement->type == ElementType::_RBEND))
      {
        willModify = true;
        angleIn -= 0.5*prevElement->angle;
      }
    }

  // Check if the component already exists and return that.
  // Don't use the registry for output elements since reliant on unique name
  // this cannot apply for sbends as it now uses individual wedge component
  // registration logic in BDSBendBuilder rather than the element as a whole.

  // TBC - this is difficult to understand.  Also, why not RBEND too?
  if (registered && !willModify && (element->type != ElementType::_SBEND))
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
  G4ThreeVector outputFaceNormal = faces.second.rotateZ(nextTilt - currentTilt);
  
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
    component = CreateSBend(); break;
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
    component = CreateThinMultipole(angleIn); break;
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
  case ElementType::_TRANSFORM3D:
    component = CreateTransform3D(); break;
  case ElementType::_AWAKESCREEN:
#ifdef USE_AWAKE
    component = CreateAwakeScreen(); break; 
#else
    G4cerr << __METHOD_NAME__ << "Awake Screen can't be used - not compiled with AWAKE module!" << G4endl;
    exit(1);
#endif
  case ElementType::_AWAKESPECTROMETER:
#ifdef USE_AWAKE
    component = CreateAwakeSpectrometer(); break;
#else
    G4cerr << __METHOD_NAME__ << "Awake Spectrometer can't be used - not compiled with AWAKE module!" << G4endl;
    exit(1);
#endif
    
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
  // if it both the component didn't exist and it has been constructed
  if (component)
    {
      component->SetBiasVacuumList(element->biasVacuumList);
      component->SetBiasMaterialList(element->biasMaterialList);
      component->SetRegion(element->region);
      SetFieldDefinitions(element, component);
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

BDSAcceleratorComponent* BDSComponentFactory::CreateSBend()
  {
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}

  PoleFaceRotationsNotTooLarge(element);  // check if poleface is not too large

  // require drift next to non-zero poleface or sbend with matching poleface
  if (BDS::IsFinite(element->e1))
    {
      if (prevElement &&
          prevElement->type != ElementType::_DRIFT &&
	  prevElement->type != ElementType::_THINMULT &&
	  !(prevElement->type == ElementType::_SBEND && !BDS::IsFinite(prevElement->e2 + element->e1)))
	{
	  G4cerr << __METHOD_NAME__ << "SBend " << element->name
		 << " has a non-zero incoming poleface "
		 << "which requires the previous element to be a Drift or SBend"
		 << " with opposite outcoming poleface" << G4endl;
	  exit(1);
	}
    }

  if (BDS::IsFinite(element->e2))
    {
      if (nextElement &&
	  nextElement->type != ElementType::_DRIFT &&
	  nextElement->type != ElementType::_THINMULT &&
	  !(nextElement->type == ElementType::_SBEND && !BDS::IsFinite(nextElement->e1 + element->e2)))
	{
	  G4cerr << __METHOD_NAME__ << "SBend " << element->name
		 << " has a non-zero outgoing poleface  "
		 << " which requires the next element to be a Drift or SBend"
		 << " with opposite incoming poleface" << G4endl;
	  exit(1);
	}
    }

  G4double length = element->l * CLHEP::m;
  BDSMagnetStrength *st = new BDSMagnetStrength();
  if (BDS::IsFinite(element->B) &&
      BDS::IsFinite(element->angle))
    {// both are specified and should be used - under or overpowered dipole by design
      (*st)["field"] = element->B;
      (*st)["angle"] = -element->angle;
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
      (*st)["angle"] = -element->angle;
      (*st)["field"] = -brho * (*st)["angle"] / length * charge * ffact / CLHEP::tesla / CLHEP::m;
    }

  // Quadrupole component
  if (BDS::IsFinite(element->k1))
    {(*st)["k1"] = element->k1 / CLHEP::m2;}

#ifdef BDSDEBUG
  G4cout << "Angle " << (*st)["angle"] << G4endl;
  G4cout << "Field " << (*st)["field"] << G4endl;
#endif

  G4double angleIn = element->e1 - 0.5*element->angle;
  G4double angleOut = element->e2 - 0.5*element->angle;


  BDSLine *sbendline = BDSBendBuilder::Instance()->SBendLine(element,
                                                             angleIn,
                                                             angleOut,
                                                             st);
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
      if (prevElement &&
	  prevElement->type != ElementType::_DRIFT &&
          prevElement->type != ElementType ::_THINMULT &&
          !(prevElement->type == ElementType::_RBEND && !BDS::IsFinite(prevElement->e2 + element->e1) ))
	{
	  G4cerr << __METHOD_NAME__ << "RBend " << element->name
		 << " has a non-zero incoming poleface "
		 << "which requires the previous element to be a Drift or RBend"
		 << " with opposite outcoming poleface" << G4endl;
	  exit(1);
	}
    }

  if (BDS::IsFinite(element->e2))
    {
      if (nextElement &&
	  nextElement->type != ElementType::_DRIFT &&
          nextElement->type != ElementType ::_THINMULT &&
          !(nextElement->type == ElementType::_RBEND && !BDS::IsFinite(nextElement->e1 + element->e2) ))
	{
	  G4cerr << __METHOD_NAME__ << "RBend " << element->name
		 << " has a non-zero outgoing poleface  "
		 << " which requires the next element to be a Drift or RBend"
		 << " with opposite incoming poleface" << G4endl;
	  exit(1);
	}
    }

  // calculate length of central straight length and edge sections
  // unfortunately, this has to be duplicated here as we need to
  // calculated the magnetic field length (less than the full length)
  // in case we need to calculate the field
  G4double outerRadius = PrepareOuterDiameter(element)*0.5;
  G4double angle       = element->angle;
  G4double length      = element->l*CLHEP::m;

  CheckBendLengthAngleWidthCombo(length, angle, 2*outerRadius, element->name);

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

  BDSLine* rbendline = BDSBendBuilder::Instance()->RBendLine(element,
                                                             prevElement,
                                                             nextElement,
                                                             angleIn,
                                                             angleOut,
                                                             brho,
                                                             st);
  return rbendline;
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
  (*st)["k1"] = element->k1;
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
  (*st)["k2"] = element->k2;
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
  (*st)["k3"] = element->k3;
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
  (*st)["k4"] = element->k4;
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

BDSAcceleratorComponent* BDSComponentFactory::CreateThinMultipole(G4double angleIn)
{
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
     (*st)[*nkey] = (*kn);
     (*st)[*skey] = (*ks);
   }
  
  BDSBeamPipeInfo* beamPipeInfo = PrepareBeamPipeInfo(element, -angleIn, angleIn);
  BDSMagnetOuterInfo* magnetOuterInfo = PrepareMagnetOuterInfo(element, -angleIn, angleIn);
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
  
  thinMultipole->SetExtent(BDSExtent(beamPipeInfo->aper1,
				     beamPipeInfo->aper1,
				     thinElementLength*0.5));

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
	 << " precision region " << element->region
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
      (*st)["ks"]    = (*st)["field"] / brho;
    }
  else
    {
      (*st)["field"] = (element->ks / CLHEP::m) * brho;
      (*st)["ks"]    = element->ks;
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
	 << " angle=" << element->angle/CLHEP::rad<<"rad"
	 << " precision region " << element->region
	 << G4endl;
#endif
  G4TwoVector size;
  size.setX(element->screenXSize*CLHEP::m);
  size.setY(element->screenYSize*CLHEP::m);
  G4cout << __METHOD_NAME__ << " - size = " << size << G4endl;
  
  BDSScreen* theScreen = new BDSScreen( element->name,
					element->l*CLHEP::m,
					PrepareBeamPipeInfo(element),
					size,
					element->angle); 
  if(element->layerThicknesses.size() != element->layerMaterials.size())
    {
      std::stringstream ss;
      ss << "Material and thicknesses lists are of unequal size.";
      ss<< element->layerMaterials.size() << " and " << element->layerThicknesses.size();
      G4Exception(ss.str().c_str(), "-1", FatalException, "");
    }
  if( (element->layerThicknesses.size() != element->layerIsSampler.size()) && ( element->layerIsSampler.size() !=0 ))
    {
      std::stringstream ss;
      ss << "Material and isSampler lists are of unequal size.";
      ss<< element->layerMaterials.size() << " and " << element->layerIsSampler.size();
      G4Exception(ss.str().c_str(), "-1", FatalException, "");
    }

  if(element->layerThicknesses.size() == 0 )
    {G4Exception("Number of screen layers = 0.", "-1", FatalException, "");}
  
  std::list<std::string>::const_iterator itm;
  std::list<double>::const_iterator itt;
  std::list<int>::const_iterator itIsSampler;
  for(itt = element->layerThicknesses.begin(),
	itm = element->layerMaterials.begin(),
	itIsSampler = element->layerIsSampler.begin();
      itt != element->layerThicknesses.end();
      itt++, itm++)
    {
      G4cout << __METHOD_NAME__ << " - screeen layer: thickness: " << 
	*(itt)<< ", material "  << (*itm) << 
	", isSampler: "  << (*itIsSampler) << G4endl;
      if(element->layerIsSampler.size()>0)
	{
	  theScreen->screenLayer((*itt)*CLHEP::m, *itm, *itIsSampler);
	  itIsSampler++;
	}
      else
	{theScreen->screenLayer((*itt)*CLHEP::m, *itm);}
    }
  return theScreen;
}

#ifdef USE_AWAKE
BDSAcceleratorComponent* BDSComponentFactory::CreateAwakeScreen()
{
#ifdef BDSDEBUG 
  G4cout << "---->creating Awake Screen,"
	 << "twindow = " << element->twindow*1e3/CLHEP::um << " um"
	 << "tscint = " << element->tscint*1e3/CLHEP::um << " um"
	 << "windowScreenGap = " << element->windowScreenGap*1e3/CLHEP::um << " um"
	 << "windowmaterial = " << element->windowmaterial << " um"
	 << "scintmaterial = " << element->scintmaterial << " um"
	 << G4endl;
#endif
  return (new BDSAwakeScintillatorScreen(element->name,
					 element->scintmaterial,
					 element->tscint*1e3,
					 element->windowScreenGap*1e3,
					 element->angle,
					 element->twindow*1e3,
					 element->windowmaterial));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateAwakeSpectrometer()
{
#ifdef BDSDEBUG 
  G4cout << "---->creating AWAKE spectrometer,"
	 << "twindow = " << element->twindow*1e3/CLHEP::um << " um"
	 << "tscint = " << element->tscint*1e3/CLHEP::um << " um"
     << "screenPSize = " << _element.screenPSize*1e3/CLHEP::um << " um"
	 << "windowScreenGap = " << element->windowScreenGap*1e3/CLHEP::um << " um"
	 << "windowmaterial = " << element->windowmaterial << " um"
     << "tmount = " << element->tmount*1e3/CLHEP::um << " um"
     << "mountmaterial = " << _element.mountmaterial << " um"	
	 << "scintmaterial = " << element->scintmaterial << " um"
	 << G4endl;
#endif
  BDSFieldInfo* awakeField = nullptr;
  if (element->bmapFile.empty())
    {
      BDSMagnetStrength* awakeStrength = new BDSMagnetStrength(); 
      (*awakeStrength)["field"] = element->B;
      
      awakeField = new BDSFieldInfo(BDSFieldType::dipole,
				    brho,
				    BDSIntegratorType::g4nystromrk4,
				    awakeStrength);
    }
  else
    {
      awakeField = new BDSFieldInfo(BDSFieldType::threed,
				    brho,
				    BDSIntegratorType::g4nystromrk4,
				    nullptr,
				    true,
				    G4Transform3D(),
				    nullptr,
				    element->bmapFile);
    }
  return (new BDSAwakeSpectrometer(element->name,
				   element->l*1e3,
				   awakeField,
				   element->poleStartZ*1e3,
				   element->scintmaterial,
				   element->tscint*1e3,
                   element->screenPSize*1e3,
				   element->windowScreenGap*1e3,
				   element->angle,
				   element->twindow*1e3,
				   element->windowmaterial,
                   element->tmount*1e3,
                   element->mountmaterial,
				   element->screenEndZ*1e3,
				   element->spec,
				   element->screenWidth*1e3));
}
#endif

BDSAcceleratorComponent* BDSComponentFactory::CreateTransform3D()
{
	
#ifdef BDSDEBUG 
  G4cout << "---->creating Transform3d,"
	 << " name= "  << element->name
	 << " xdir= "  << element->xdir/CLHEP::m    << "m"
	 << " ydir= "  << element->ydir/CLHEP::m    << "m"
	 << " zdir= "  << element->zdir/CLHEP::m    << "m"
	 << " phi= "   << element->phi/CLHEP::rad   << "rad"
	 << " theta= " << element->theta/CLHEP::rad << "rad"
	 << " psi= "   << element->psi/CLHEP::rad   << "rad"
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
    {
      info->geometryType = BDS::DetermineMagnetGeometryType(element->magnetGeometryType);
      info->geometryTypeAndPath = element->magnetGeometryType;
    }

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

  if ((element->angle < 0) && (element->yokeOnInside))
    {info->yokeOnLeft = true;}
  else if ((element->angle > 0) && (!(element->yokeOnInside)))
    {info->yokeOnLeft = true;}
  else
    {info->yokeOnLeft = false;}
      
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

BDSBeamPipeInfo* BDSComponentFactory::PrepareBeamPipeInfo(Element const* element,
							  const G4double angleIn,
							  const G4double angleOut) const
{
  auto faces = BDS::CalculateFaces(angleIn, angleOut);
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
					      faces.first,
					      faces.second);
  return info;
}

BDSTiltOffset* BDSComponentFactory::CreateTiltOffset(Element const* element) const
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "offsetX,Y: " << element->offsetX << " " << element->offsetY << " tilt: " << element->tilt << G4endl;
#endif
  G4double xOffset = element->offsetX * CLHEP::m;
  G4double yOffset = element->offsetY * CLHEP::m;
  G4double tilt    = element->tilt    * CLHEP::rad;

  BDSTiltOffset* result = nullptr;
  if (BDS::IsFinite(xOffset) || BDS::IsFinite(yOffset) || BDS::IsFinite(tilt))
    {result = new BDSTiltOffset(xOffset, yOffset, tilt);}
  return result;
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

void BDSComponentFactory::SetFieldDefinitions(Element const* element,
					      BDSAcceleratorComponent* component) const
{
  if (BDSMagnet* mag = dynamic_cast<BDSMagnet*>(component))
    {
      if (!(element->fieldAll.empty()))
	{
	  G4cerr << "Error: Magnet named \"" << element->name
		 << "\" is a magnet, but has fieldAll defined." << G4endl
		 << "Can only have fieldOuter or fieldInner specified." << G4endl;
	  exit(1);
	}
      if (!(element->fieldOuter.empty())) // ie variable isn't ""
	{mag->SetOuterField(BDSFieldFactory::Instance()->GetDefinition(element->fieldOuter));}
      if (!(element->fieldVacuum.empty()))
	{mag->SetVacuumField(BDSFieldFactory::Instance()->GetDefinition(element->fieldVacuum));}
    }
  else
    {
      if (!(element->fieldAll.empty()))
	{component->SetField(BDSFieldFactory::Instance()->GetDefinition(element->fieldAll));}
    }
}
