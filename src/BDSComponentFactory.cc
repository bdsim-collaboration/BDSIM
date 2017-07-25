#include "BDSComponentFactory.hh"

// elements
#ifdef USE_AWAKE
#include "BDSAwakeScintillatorScreen.hh"
#include "BDSAwakeSpectrometer.hh"
#endif
#include "BDSCavityElement.hh"
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
#include "BDSShield.hh"
#include "BDSTeleporter.hh"
#include "BDSTerminator.hh"
#include "BDSTiltOffset.hh"
#include "BDSTransform3D.hh"

// general
#include "BDSAcceleratorComponentRegistry.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSBeamPipeType.hh"
#include "BDSBendBuilder.hh"
#include "BDSCavityInfo.hh"
#include "BDSCavityType.hh"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldFactory.hh"
#include "BDSFieldType.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIntegratorSet.hh"
#include "BDSIntegratorSetType.hh"
#include "BDSIntegratorType.hh"
#include "BDSMagnetOuterFactory.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetGeometryType.hh"
#include "BDSMagnetStrength.hh"
#include "BDSMagnetType.hh"
#include "BDSMaterials.hh"
#include "BDSParser.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Transform3D.hh"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "parser/elementtype.h"
#include "parser/cavitymodel.h"

#include <cmath>
#include <string>
#include <utility>
using namespace GMAD;

BDSComponentFactory::BDSComponentFactory()
{
  lengthSafety  = BDSGlobalConstants::Instance()->LengthSafety();
  brho          = BDSGlobalConstants::Instance()->BRho();
  BDSIntegratorSetType intSetType = BDSGlobalConstants::Instance()->IntegratorSet();
  integratorSet = BDS::IntegratorSet(intSetType);
  G4cout << __METHOD_NAME__ << "Using \"" << intSetType << "\" set of integrators" << G4endl;

  // prepare rf cavity model info from parser
  PrepareCavityModels();
  
  thinElementLength = BDSGlobalConstants::Instance()->ThinElementLength();
}

BDSComponentFactory::~BDSComponentFactory()
{
  for(auto info : cavityInfos)
    {delete info.second;}

  // Deleted here although not used directly here as new geometry can only be
  // created through this class.
  delete BDSBeamPipeFactory::Instance();
  delete BDSMagnetOuterFactory::Instance();
}

BDSAcceleratorComponent* BDSComponentFactory::CreateComponent(Element const* elementIn,
							      Element const* prevElementIn,
							      Element const* nextElementIn,
							      G4double currentArcLength)
{
  element = elementIn;
  prevElement = prevElementIn;
  nextElement = nextElementIn;
  G4double angleIn  = 0.0;
  G4double angleOut = 0.0;
  G4bool registered = false;
  // Used for multiple instances of the same element but different poleface rotations.
  // Ie only a drift is modified to match the pole face rotation of a magnet.
  G4bool differentFromDefinition = false;

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "named: \"" << element->name << "\"" << G4endl;  
#endif

  if (BDSAcceleratorComponentRegistry::Instance()->IsRegistered(element->name))
    {registered = true;}

  if (element->type == ElementType::_DRIFT)
    {
      // minuses are to go from 'strength convention' to 3d cartesian.
      if (prevElement)
        {angleIn  = OutgoingFaceAngle(prevElement);}
      if (nextElement)
        {angleOut = IncomingFaceAngle(nextElement);}
      
      //if drift has been modified at all
      if (BDS::IsFinite(angleIn) || BDS::IsFinite(angleOut))
	{differentFromDefinition = true;}
    }
  else if (element->type == ElementType::_RBEND)
    {// bend builder will construct it to match - but here we just now it's different
      // match a previous rbend with half the angle
      if (prevElement && (prevElement->type == ElementType::_RBEND))
	{differentFromDefinition = true;}
      // match the upcoming rbend with half the angle
      if (nextElement && (nextElement->type == ElementType::_RBEND))
	{differentFromDefinition = true;}
    }
  else if (element->type == ElementType::_THINMULT)
    {// thinmultipole only uses one angle - so angleIn
      if (prevElement && nextElement)
	{// both exist
	  ElementType prevType = prevElement->type;
	  ElementType nextType = nextElement->type;
	  if (prevType == ElementType::_DRIFT && nextType == ElementType::_DRIFT)
	    {angleIn = 0;} // between two drifts - flat
	  else if (prevType == ElementType::_DRIFT)
	    {angleIn = -IncomingFaceAngle(nextElement);} // previous is drift which will match next
	  else
	    {angleIn = OutgoingFaceAngle(prevElement);} // next is drift which will match prev
	}
      else if (prevElement)
	{angleIn = OutgoingFaceAngle(prevElement);} // only previous element - match it
      else
	{angleIn = IncomingFaceAngle(nextElement);} // only next element - match it

      // because thin multipoles adapt to what's around them, it's not possible to know
      // if, in the case the thin multipole already exists in the registry, it's been
      // modified or is flat, therefore we mark them all as unique.
      differentFromDefinition = true;
    }
  
  // Check if the component already exists and return that.
  // Don't use the registry for output elements since reliant on unique name.
  if (registered && !differentFromDefinition)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "using already manufactured component" << G4endl;
#endif
      return BDSAcceleratorComponentRegistry::Instance()->GetComponent(element->name);
    }

  // Update name for this component
  elementName = element->name;

  if (differentFromDefinition)
    {
      G4int val;
      if (modifiedElements.find(elementName) == modifiedElements.end())
	{
	  modifiedElements[elementName] = 0;
	  val = 0;
	}
      else
	{
	  val = modifiedElements[elementName] + 1;
	  modifiedElements[elementName] = val;
	}
      elementName += "_mod_" + std::to_string(val);
    }

  BDSAcceleratorComponent* component = nullptr;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " - creating \"" << elementName << "\"" << G4endl;
  element->print();
#endif
  switch(element->type){
  case ElementType::_DRIFT:
    component = CreateDrift(angleIn, angleOut); break;
  case ElementType::_RF:
    {
      component = CreateRF(currentArcLength);
      differentFromDefinition = true; // unique phase for every placement in beam line
      break;
    }
  case ElementType::_SBEND:
    component = CreateSBend(); break;
  case ElementType::_RBEND:
    component = CreateRBend(); break;
  case ElementType::_HKICKER:
    component = CreateKicker(KickerType::horizontal); break;
  case ElementType::_VKICKER:
    component = CreateKicker(KickerType::vertical); break;
  case ElementType::_KICKER:
  case ElementType::_TKICKER:
    component = CreateKicker(KickerType::general); break;
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
  case ElementType::_SHIELD:
    component = CreateShield(); break;
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
    G4cerr << __METHOD_NAME__ << "unknown type " << element->type << G4endl;
    exit(1);
    break;
  }

  // note this test will only be reached (and therefore the component registered)
  // if both the component didn't exist and it has been constructed
  if (component)
    {
      component->SetBiasVacuumList(element->biasVacuumList);
      component->SetBiasMaterialList(element->biasMaterialList);
      component->SetRegion(element->region);
      SetFieldDefinitions(element, component);
      component->Initialise();
      // register component and memory
      BDSAcceleratorComponentRegistry::Instance()->RegisterComponent(component,differentFromDefinition);
    }
  
  return component;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateTeleporter(const G4ThreeVector teleporterDelta)
{
  G4Transform3D transform = G4Transform3D(G4RotationMatrix(), teleporterDelta);
  
  BDSFieldInfo* vacuumFieldInfo = new BDSFieldInfo(BDSFieldType::teleporter,
						   brho,
						   BDSIntegratorType::teleporter,
						   nullptr,    // magnet strength doesn't apply
						   true,       // provide global translation
						   transform);
  
  G4cout << "---->creating Teleporter, "
	 << "l = " << teleporterDelta.z()/CLHEP::m << "m"
	 << G4endl;

  return( new BDSTeleporter(teleporterDelta.z(), vacuumFieldInfo));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateDrift(G4double angleIn, G4double angleOut)
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  // Create normal vectors
  std::pair<G4ThreeVector,G4ThreeVector> faces = BDS::CalculateFaces(angleIn, angleOut);

  // current element tilt
  G4double currentTilt  = element->tilt * CLHEP::rad;
  G4double prevTilt = 0;
  G4double nextTilt = 0;
  if (prevElement)
    {prevTilt = prevElement->tilt * CLHEP::rad;}
  if (nextElement)
    {nextTilt = nextElement->tilt * CLHEP::rad;}
  G4ThreeVector inputFaceNormal  = faces.first.rotateZ(prevTilt - currentTilt);
  G4ThreeVector outputFaceNormal = faces.second.rotateZ(nextTilt - currentTilt);

  const G4double length = element->l*CLHEP::m;

  // Beampipeinfo needed here to get aper1 for check.
  BDSBeamPipeInfo* beamPipeInfo = PrepareBeamPipeInfo(element, inputFaceNormal,
						      outputFaceNormal);

  const BDSExtent indicativeExtent = beamPipeInfo->IndicativeExtent();
  G4bool facesWillIntersect = BDS::WillIntersect(inputFaceNormal, outputFaceNormal,
						 length, indicativeExtent, indicativeExtent);

  if (facesWillIntersect)
    {
      G4cerr << __METHOD_NAME__ << "Drift \"" << elementName
	     << "\" between \"";
      if (prevElement)
	{G4cerr << prevElement->name;}
      else
	{G4cerr << "none";}
      G4cerr << "\" and \"";
      if (nextElement)
	{G4cerr << nextElement->name;}
      else
	{G4cerr << "none";}
      G4cerr << "\" is too short given its width and the angle of its faces." << G4endl;
      exit(1);
    }

  return (new BDSDrift( elementName,
			length,
			beamPipeInfo));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateRF(G4double currentArcLength)
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSIntegratorType intType = integratorSet->Integrator(BDSFieldType::rfcavity);

  BDSFieldType fieldType = BDSFieldType::rf; // simple sinusoidal E field only
  if (!(element->fieldVacuum.empty()))
    {
      BDSFieldInfo* field = BDSFieldFactory::Instance()->GetDefinition(element->fieldVacuum);
      fieldType = field->FieldType();
    }

  BDSMagnetStrength* st = PrepareCavityStrength(element, currentArcLength);
  G4Transform3D fieldTrans = CreateFieldTransform(element);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(fieldType,
					       brho,
					       intType,
					       st,
					       true,
					       fieldTrans);
  
  BDSCavityInfo* cavityInfo = PrepareCavityModelInfo(element, (*st)["frequency"]);

  // update 0 point of field with geometry
  (*st)["equatorRadius"] = cavityInfo->equatorRadius;
  G4Material* vacuumMaterial = PrepareVacuumMaterial(element);
    
  return new BDSCavityElement(elementName,
			      element->l*CLHEP::m,
			      vacuumMaterial,
			      vacuumField,
			      cavityInfo);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateSBend()
  {
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}

  PoleFaceRotationsNotTooLarge(element);

  // don't check here on whether the possibly next / previous sbend will clash with
  // pole face angles - let that be checked after element construction in the beamline

  BDSMagnetStrength* st = new BDSMagnetStrength();
  G4double angle = 0;
  G4double field = 0;
  CalculateAngleAndFieldSBend(element, angle, field);
  (*st)["angle"]  = angle;
  (*st)["field"]  = field;
  (*st)["length"] = element->l * CLHEP::m; // arc length

  // Quadrupole component
  if (BDS::IsFinite(element->k1))
    {(*st)["k1"] = element->k1 / CLHEP::m2;}

#ifdef BDSDEBUG
  G4cout << "Angle " << (*st)["angle"] << G4endl;
  G4cout << "Field " << (*st)["field"] << G4endl;
#endif
  
  auto sBendLine = BDS::BuildSBendLine(element, st, brho, integratorSet);
  
  return sBendLine;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateRBend()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  PoleFaceRotationsNotTooLarge(element);

  // don't check here on whether the possibly next / previous sbend will clash with
  // pole face angles - let that be checked after element construction in the beamline

  BDSMagnetStrength* st = new BDSMagnetStrength();  
  G4double arcLength = 0, chordLength = 0, field = 0, angle = 0;
  CalculateAngleAndFieldRBend(element, arcLength, chordLength, field, angle);
  
  (*st)["angle"]  = angle;
  (*st)["field"]  = field;
  (*st)["length"] = arcLength;

  // Check the faces won't overlap due to too strong an angle with too short a magnet
  G4double outerDiameter = PrepareOuterDiameter(element);
  CheckBendLengthAngleWidthCombo(arcLength, (*st)["angle"], outerDiameter, elementName);

  // Quadrupole component
  if (BDS::IsFinite(element->k1))
    {(*st)["k1"] = element->k1 / CLHEP::m2;}

  BDSLine* rbendline = BDS::BuildRBendLine(element,
					   prevElement,
					   nextElement,
					   brho,
					   st,
					   integratorSet);
  return rbendline;
}
void BDSComponentFactory::GetKickValue(G4double& hkick,
				       G4double& vkick,
				       const KickerType type) const
{
  G4bool kickFinite = BDS::IsFinite(element->kick);
  switch (type)
    {
    case KickerType::horizontal:
      {
	hkick = kickFinite ? element->kick : element->hkick;
	// backwards compatability - if both are zero but angle if finite
	// for this element - use that.
	if (!BDS::IsFinite(hkick) && BDS::IsFinite(element->angle))
          {hkick = element->angle;} //+ve to match hkick definition
	vkick = 0;
	break;
      }
    case KickerType::vertical:
      {
	vkick = kickFinite ? element->kick : element->vkick;
	// backwards compatability - if both are zero but angle if finite
	// for this element - use that.
	if (!BDS::IsFinite(vkick) && BDS::IsFinite(element->angle))
          {vkick = element->angle;} //+ve to match vkick definition
	hkick = 0;
	break;
      }
    case KickerType::general:
      {
	hkick = element->hkick;
	vkick = element->vkick;
	// element->kick will be ignored
	if (BDS::IsFinite(element->kick))
	  {
	    G4cout << __METHOD_NAME__ << "Warning: 'kick' defined in element"
		   << "\"" << element->name << "\" but will be ignored as general kicker"
		   << G4endl;
	  }
      }
    default:
      {break;}
    }
}

BDSAcceleratorComponent* BDSComponentFactory::CreateKicker(KickerType type)
{
  BDSMagnetStrength* st         = new BDSMagnetStrength();
  BDSFieldType       fieldType  = BDSFieldType::dipole3d;
  BDSIntegratorType  intType    = BDSIntegratorType::g4classicalrk4; // default
  G4double           chordLength;
  
  if(!HasSufficientMinimumLength(element, false)) // false for don't print warning
    {// thin kicker
      fieldType   = BDSFieldType::bzero;
      intType     = BDSIntegratorType::kickerthin;
      chordLength = thinElementLength;
      G4double hkick = 0;
      G4double vkick = 0;
      GetKickValue(hkick, vkick, type);
      (*st)["hkick"] = hkick;
      (*st)["vkick"] = vkick;
    }
  else
    {// thick kicker
      chordLength = element->l*CLHEP::m;
      // sin(angle) = dP -> angle = sin^-1(dP)
      G4double          hkick = 0;
      G4double          vkick = 0;
      GetKickValue(hkick, vkick, type);
      G4double         angleX = asin(hkick);
      G4double         angleY = asin(vkick);

      // Setup result variables - 'x' and 'y' refer to the components along the direction
      // the particle will change. These will therefore not be Bx and By.
      G4double fieldX = 0;
      G4double fieldY = 0;

      if (BDS::IsFinite(angleX))
	{// with comments
	  // calculate the chord length of the arc through the field from the straight
	  // ahead length for this element which here is 'chordLength'.
	  G4double fieldChordLengthX = chordLength / cos(0.5*angleX);

	  // now calculate the bending radius
	  G4double bendingRadiusX = fieldChordLengthX * 0.5 / sin(std::abs(angleX) * 0.5);
	  
	  // no calculate the arc length of the trajectory based on each bending radius
	  G4double arcLengthX = std::abs(bendingRadiusX * angleX);

	  // -ve here in horizontal only for convention matching
	  fieldX = FieldFromAngle(-angleX, arcLengthX);
	} // else fieldX default is 0

      if (BDS::IsFinite(angleY))
	{// same as x, no need for comments
	  G4double fieldChordLengthY = chordLength / cos(0.5*angleY);
	  G4double bendingRadiusY    = fieldChordLengthY * 0.5 / sin(std::abs(angleY) * 0.5);
	  G4double arcLengthY        = std::abs(bendingRadiusY * angleY);
	  fieldY = FieldFromAngle(angleY,  arcLengthY);
	}
      
      // note field for kick in x is unit Y, hence B = (y,x,0) here
      G4ThreeVector     field = G4ThreeVector(fieldY, fieldX, 0);
      G4double       fieldMag = field.mag();
      G4ThreeVector unitField = field.unit();
      
      (*st)["field"] = fieldMag;
      (*st)["bx"]    = unitField.x();
      (*st)["by"]    = unitField.y();
    }
  
  BDSMagnetType t;
  switch (type)
    {
    case KickerType::horizontal:
    case KickerType::general:
      {t = BDSMagnetType::hkicker; break;}
    case KickerType::vertical:
      {t = BDSMagnetType::vkicker; break;}
    default:
      {t = BDSMagnetType::hkicker; break;}
    }

  G4Transform3D fieldTrans = CreateFieldTransform(element);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(fieldType,
					       brho,
					       intType,
					       st,
					       true,
					       fieldTrans);

  G4bool yokeOnLeft = YokeOnLeft(element, st);
  
  return new BDSMagnet(t,
		       elementName,
		       chordLength,
		       PrepareBeamPipeInfo(element),
		       PrepareMagnetOuterInfo(element, 0, 0, yokeOnLeft),
		       vacuumField);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateQuad()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["k1"] = element->k1;

  return CreateMagnet(st, BDSFieldType::quadrupole, BDSMagnetType::quadrupole);
}  
  
BDSAcceleratorComponent* BDSComponentFactory::CreateSextupole()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["k2"] = element->k2;

  return CreateMagnet(st, BDSFieldType::sextupole, BDSMagnetType::sextupole);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateOctupole()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["k3"] = element->k3;

  return CreateMagnet(st, BDSFieldType::octupole, BDSMagnetType::octupole);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateDecapole()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["k4"] = element->k4;

  return CreateMagnet(st, BDSFieldType::decapole, BDSMagnetType::decapole);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateMultipole()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}
  
  BDSMagnetStrength* st = PrepareMagnetStrengthForMultipoles(element);
  
  return CreateMagnet(st, BDSFieldType::multipole, BDSMagnetType::multipole,
		      (*st)["angle"]); // multipole could bend beamline
}

BDSAcceleratorComponent* BDSComponentFactory::CreateThinMultipole(G4double angleIn)
{
  BDSMagnetStrength* st = PrepareMagnetStrengthForMultipoles(element);
  BDSBeamPipeInfo* beamPipeInfo = PrepareBeamPipeInfo(element, angleIn, -angleIn);
  beamPipeInfo->beamPipeType = BDSBeamPipeType::circularvacuum;
  BDSMagnetOuterInfo* magnetOuterInfo = PrepareMagnetOuterInfo(element, -angleIn, angleIn);
  magnetOuterInfo->geometryType = BDSMagnetGeometryType::none;

  BDSIntegratorType intType = integratorSet->multipolethin;
  G4Transform3D fieldTrans  = CreateFieldTransform(element);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::multipole,
					       brho,
					       intType,
					       st,
					       true,
					       fieldTrans);
  
  BDSMagnet* thinMultipole =  new BDSMagnet(BDSMagnetType::thinmultipole,
					    elementName,
					    thinElementLength,
					    beamPipeInfo,
					    magnetOuterInfo,
					    vacuumField);
  
  thinMultipole->SetExtent(BDSExtent(beamPipeInfo->aper1,
				     beamPipeInfo->aper1,
				     thinElementLength*0.5));

  return thinMultipole;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateElement()
{
  if(!HasSufficientMinimumLength(element)) 
    {return nullptr;}
  
  return (new BDSElement(elementName,
			 element->l * CLHEP::m,
			 PrepareOuterDiameter(element),
			 element->geometryFile,
			 element->fieldAll));
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

  return CreateMagnet(st, BDSFieldType::solenoid, BDSMagnetType::solenoid);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateRectangularCollimator()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  return new BDSCollimatorRectangular(elementName,
				      element->l*CLHEP::m,
				      PrepareOuterDiameter(element),
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

  return new BDSCollimatorElliptical(elementName,
				     element->l*CLHEP::m,
				     PrepareOuterDiameter(element),
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
  BDSIntegratorType intType = integratorSet->Integrator(BDSFieldType::muonspoiler);
  G4Transform3D fieldTrans = CreateFieldTransform(element);
  BDSFieldInfo* outerField = new BDSFieldInfo(BDSFieldType::muonspoiler,
					      brho,
					      intType,
					      st,
					      true,
					      fieldTrans);
  BDSFieldInfo* vacuumField = new BDSFieldInfo();

  return new BDSMagnet(BDSMagnetType::muonspoiler,
		       elementName,
		       element->l*CLHEP::m,
		       PrepareBeamPipeInfo(element),
		       PrepareMagnetOuterInfo(element, st),
		       vacuumField,
		       0,
		       outerField);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateShield()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSBeamPipeInfo* bpInfo = PrepareBeamPipeInfo(element);

  G4Material* material = BDSMaterials::Instance()->GetMaterial(element->material);

  BDSShield* shield = new BDSShield(elementName,
				    element->l*CLHEP::m,
				    PrepareOuterDiameter(element),
				    element->xsize*CLHEP::m,
				    element->ysize*CLHEP::m,
				    material,
				    bpInfo);
  return shield;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateDegrader()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

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

  return (new BDSDegrader(elementName,
			  element->l*CLHEP::m,
			  PrepareOuterDiameter(element),
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
	
  G4double length = element->l*CLHEP::m;
  G4double lambda = element->waveLength*CLHEP::m;
	
  G4ThreeVector direction = G4ThreeVector(element->xdir,element->ydir,element->zdir);
  G4ThreeVector position  = G4ThreeVector(0,0,0);
	
  return (new BDSLaserWire(elementName, length, lambda, direction) );       
}

BDSAcceleratorComponent* BDSComponentFactory::CreateScreen()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}
	
  G4TwoVector size;
  size.setX(element->screenXSize*CLHEP::m);
  size.setY(element->screenYSize*CLHEP::m);
  G4cout << __METHOD_NAME__ << " - size = " << size << G4endl;
  
  BDSScreen* theScreen = new BDSScreen( elementName,
					element->l*CLHEP::m,
					PrepareBeamPipeInfo(element),
					size,
					element->angle); 
  if(element->layerThicknesses.size() != element->layerMaterials.size())
    {
      G4cerr << __METHOD_NAME__ << "Element \"" << element->name << "\" must have the "
	     << "same number of materials as layers - check 'layerMaterials'" << G4endl;
      exit(1);
    }

  if(element->layerThicknesses.size() == 0 )
    {
      G4cerr << __METHOD_NAME__ << "Element: \"" << element->name
	     << "\" has 0 screen layers" << G4endl;
      exit(1);
    }
  
  std::list<std::string>::const_iterator itm;
  std::list<double>::const_iterator itt;
  std::list<int>::const_iterator itIsSampler;
  for(itt = element->layerThicknesses.begin(),
	itm = element->layerMaterials.begin(),
	itIsSampler = element->layerIsSampler.begin();
      itt != element->layerThicknesses.end();
      ++itt, ++itm)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " - screen layer: thickness: " 
	     << *(itt) << ", material "  << (*itm)
	     <<	", isSampler: "  << (*itIsSampler) << G4endl;
#endif
      if(element->layerIsSampler.size()>0)
	{
	  theScreen->AddScreenLayer((*itt)*CLHEP::m, *itm, *itIsSampler);
	  ++itIsSampler;
	}
      else
	{theScreen->AddScreenLayer((*itt)*CLHEP::m, *itm);}
    }
  return theScreen;
}

#ifdef USE_AWAKE
BDSAcceleratorComponent* BDSComponentFactory::CreateAwakeScreen()
{
  return (new BDSAwakeScintillatorScreen(elementName,
					 element->scintmaterial,
					 element->tscint*CLHEP::m,
					 element->windowScreenGap*CLHEP::m,
					 element->angle,
					 element->twindow*CLHEP::m,
					 element->windowmaterial));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateAwakeSpectrometer()
{
  BDSFieldInfo* awakeField = nullptr;
  if (element->fieldAll.empty())
    {
      BDSMagnetStrength* awakeStrength = new BDSMagnetStrength(); 
      (*awakeStrength)["field"] = element->B;

      G4Transform3D fieldTrans = CreateFieldTransform(element);
      awakeField = new BDSFieldInfo(BDSFieldType::dipole,
				    brho,
				    BDSIntegratorType::g4nystromrk4,
				    awakeStrength,
				    true,
				    fieldTrans);
    }
  else
    {awakeField = BDSFieldFactory::Instance()->GetDefinition(element->fieldAll);}
  return (new BDSAwakeSpectrometer(elementName,
                                   element->awakeMagnetOffsetX*CLHEP::m,
				   element->l*CLHEP::m,
				   awakeField,
				   element->poleStartZ*CLHEP::m,
				   element->scintmaterial,
				   element->tscint*CLHEP::m,
				   element->screenPSize*CLHEP::m,
				   element->windowScreenGap*CLHEP::m,
				   element->angle,
				   element->twindow*CLHEP::m,
				   element->windowmaterial,
				   element->tmount*CLHEP::m,
				   element->mountmaterial,
				   element->screenEndZ*CLHEP::m,
				   element->spec,
				   element->screenWidth*CLHEP::m));
}
#endif // end of USE_AWAKE

BDSAcceleratorComponent* BDSComponentFactory::CreateTransform3D()
{
  return (new BDSTransform3D( elementName,
			      element->xdir *CLHEP::m,
			      element->ydir *CLHEP::m,
			      element->zdir *CLHEP::m,
			      element->phi *CLHEP::rad,
			      element->theta *CLHEP::rad,
			      element->psi *CLHEP::rad ) );
	
}

BDSAcceleratorComponent* BDSComponentFactory::CreateTerminator()
{
#ifdef BDSDEBUG
  G4cout << "---->creating Terminator" << G4endl;
#endif
  return new BDSTerminator();
}

BDSMagnet* BDSComponentFactory::CreateMagnet(BDSMagnetStrength* st,
					     BDSFieldType fieldType,
					     BDSMagnetType magnetType,
					     G4double angle) const
{
  BDSIntegratorType intType = integratorSet->Integrator(fieldType);
  G4Transform3D fieldTrans  = CreateFieldTransform(element);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(fieldType,
					       brho,
					       intType,
					       st,
					       true,
					       fieldTrans);

  return new BDSMagnet(magnetType,
		       elementName,
		       element->l * CLHEP::m,
		       PrepareBeamPipeInfo(element),
		       PrepareMagnetOuterInfo(element, st),
		       vacuumField,
		       angle);
}

G4bool BDSComponentFactory::HasSufficientMinimumLength(Element const* el,
						       const G4bool printWarning)
{
  if(el->l*CLHEP::m < 1e-7)
    {
      if (printWarning)
	{
	  G4cerr << "---->NOT creating element, "
		 << " name = " << el->name
		 << ", LENGTH TOO SHORT:"
		 << " l = " << el->l*CLHEP::um << "um"
		 << G4endl;
	}
      return false;
    }
  else
    {return true;}
}

void BDSComponentFactory::PoleFaceRotationsNotTooLarge(Element const* element,
						       G4double maxAngle)
{
  if (std::abs(element->e1) > maxAngle)
    {
      G4cerr << __METHOD_NAME__ << "Pole face angle e1: " << element->e1 << " is greater than " << maxAngle << G4endl;
      exit(1);
    }
  if (std::abs(element->e2) > maxAngle)
    {
      G4cerr << __METHOD_NAME__ << "Pole face angle e2: " << element->e2 << " is greater than " << maxAngle << G4endl;
      exit(1);
    }
}

G4bool BDSComponentFactory::YokeOnLeft(const Element*           element,
				       const BDSMagnetStrength* st)
{
  G4double angle = (*st)["angle"];
  G4bool yokeOnLeft;
  if ((angle < 0) && (element->yokeOnInside))
    {yokeOnLeft = true;}
  else if ((angle > 0) && (!(element->yokeOnInside)))
    {yokeOnLeft = true;}
  else
    {yokeOnLeft = false;}
  return yokeOnLeft;
}

BDSMagnetOuterInfo* BDSComponentFactory::PrepareMagnetOuterInfo(const Element* element,
								const BDSMagnetStrength* st)
{
  G4bool yokeOnLeft = YokeOnLeft(element,st);
  G4double    angle = (*st)["angle"];
  
  return PrepareMagnetOuterInfo(element, 0.5*angle, 0.5*angle, yokeOnLeft);
}

BDSMagnetOuterInfo* BDSComponentFactory::PrepareMagnetOuterInfo(const Element* element,
								const G4double angleIn,
								const G4double angleOut,
								const G4bool   yokeOnLeft)
{
  BDSMagnetOuterInfo* info = new BDSMagnetOuterInfo();
  
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
  info->outerDiameter = PrepareOuterDiameter(element);

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

  // yoke direction
  info->yokeOnLeft = yokeOnLeft;

  return info;
}

G4double BDSComponentFactory::PrepareOuterDiameter(Element const* el)
{
  G4double outerDiameter = el->outerDiameter*CLHEP::m;
  if (outerDiameter < 1e-6)
    {//outerDiameter not set - use global option as default
      outerDiameter = BDSGlobalConstants::Instance()->OuterDiameter();
    }
  return outerDiameter;
}

G4Material* BDSComponentFactory::PrepareVacuumMaterial(Element const* el) const
{
  G4Material* result;
  if (el->vacuumMaterial == "")
    {result = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->VacuumMaterial());}
  else
    {result = BDSMaterials::Instance()->GetMaterial(element->vacuumMaterial);}
  return result;
}

BDSBeamPipeInfo* BDSComponentFactory::PrepareBeamPipeInfo(Element const* el,
							  const G4ThreeVector inputFaceNormalIn,
							  const G4ThreeVector outputFaceNormalIn)
{
  BDSBeamPipeInfo* defaultModel = BDSGlobalConstants::Instance()->GetDefaultBeamPipeModel();
  BDSBeamPipeInfo* result; 
  if (!BDSGlobalConstants::Instance()->IgnoreLocalAperture())
    {
      result = new BDSBeamPipeInfo(defaultModel,
				   el->apertureType,
				   el->aper1 * CLHEP::m,
				   el->aper2 * CLHEP::m,
				   el->aper3 * CLHEP::m,
				   el->aper4 * CLHEP::m,
				   el->vacuumMaterial,
				   el->beampipeThickness * CLHEP::m,
				   el->beampipeMaterial,
				   inputFaceNormalIn,
				   outputFaceNormalIn);
    }
  else
    {// ignore the aperture model from the element and use the global one
      result = new BDSBeamPipeInfo(*defaultModel); // ok as only pointers to materials
      result->inputFaceNormal  = inputFaceNormalIn;
      result->outputFaceNormal = outputFaceNormalIn;
    }
  return result;
}

BDSBeamPipeInfo* BDSComponentFactory::PrepareBeamPipeInfo(Element const* el,
							  const G4double angleIn,
							  const G4double angleOut)
{
  auto faces = BDS::CalculateFaces(angleIn, angleOut);
  BDSBeamPipeInfo* info = PrepareBeamPipeInfo(el, faces.first, faces.second);
  return info;
}

BDSTiltOffset* BDSComponentFactory::CreateTiltOffset(Element const* el)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "offsetX,Y: " << el->offsetX << " " << el->offsetY << " tilt: " << el->tilt << G4endl;
#endif
  G4double xOffset = el->offsetX * CLHEP::m;
  G4double yOffset = el->offsetY * CLHEP::m;
  G4double tilt    = el->tilt    * CLHEP::rad;

  BDSTiltOffset* result = nullptr;
  if (BDS::IsFinite(xOffset) || BDS::IsFinite(yOffset) || BDS::IsFinite(tilt))
    {result = new BDSTiltOffset(xOffset, yOffset, tilt);}
  return result;
}

G4Transform3D BDSComponentFactory::CreateFieldTransform(const BDSTiltOffset* tiltOffset)
{
  return tiltOffset ? tiltOffset->Transform3D() : G4Transform3D();
}

G4Transform3D BDSComponentFactory::CreateFieldTransform(Element const* el)
{
  BDSTiltOffset* to = CreateTiltOffset(el);
  G4Transform3D trans = CreateFieldTransform(to);
  delete to;
  return trans;
}

void BDSComponentFactory::CheckBendLengthAngleWidthCombo(G4double arcLength,
							 G4double angle,
							 G4double outerDiameter,
							 G4String name)
{
  G4double radiusFromAngleLength =  std::abs(arcLength / angle); // s = r*theta -> r = s/theta
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
      // material can either be specified in 
      G4Material* material = nullptr;
      if (!model.material.empty())
	{material = BDSMaterials::Instance()->GetMaterial(model.material);}
	  
      auto info = new BDSCavityInfo(BDS::DetermineCavityType(model.type),
				    material,
				    model.irisRadius*CLHEP::m,
				    model.thickness*CLHEP::m,
				    model.equatorRadius*CLHEP::m,
				    model.halfCellLength*CLHEP::m,
				    model.numberOfPoints,
				    model.numberOfCells,
				    model.equatorHorizontalAxis*CLHEP::m,
				    model.equatorVerticalAxis*CLHEP::m,
				    model.irisHorizontalAxis*CLHEP::m,
				    model.irisVerticalAxis*CLHEP::m,
				    model.tangentLineAngle);
      
      cavityInfos[model.name] = info;
    }
}

BDSCavityInfo* BDSComponentFactory::PrepareCavityModelInfo(Element const* el,
							   G4double frequency) const
{
  // If the cavity model name (identifier) has been defined, return a *copy* of
  // that model - so that the component will own that info object.

  G4String modelName = G4String(el->cavityModel);

  // no specific model - prepare a default based on element parameters
  if (modelName == "")
    {return PrepareCavityModelInfoForElement(el, frequency);}

  // cavity model name specified - match up with parser object already translated here
  auto result = cavityInfos.find(modelName);
  if (result == cavityInfos.end())
    {
      G4cout << "Unknown cavity model identifier \"" << el->cavityModel << "\" - please define it" << G4endl;
      exit(1);
    }

  // we make a per element copy of the definition
  BDSCavityInfo* info = new BDSCavityInfo(*(result->second));

  // If no material specified, we take the material from the element. If no material at
  // all, we exit with warning.
  if (!info->material)
    {
      if (el->material.empty())
	{
	  G4cout << "ERROR: Cavity material is not defined for cavity \"" << elementName << "\""
		 << "or for cavity model \"" << el->cavityModel << "\" - please define it" << G4endl;
	  exit(1);
	}
      else
	{info->material = BDSMaterials::Instance()->GetMaterial(el->material);}
    }

  return info;
}

BDSCavityInfo* BDSComponentFactory::PrepareCavityModelInfoForElement(Element const* el,
								     G4double frequency) const
{
  /// prepare aperture information for this element to base default cavity on.
  BDSBeamPipeInfo* aperture = PrepareBeamPipeInfo(el);

  G4double aper1     = aperture->aper1;
  G4double outerD    = PrepareOuterDiameter(el);
  G4double defaultOuterD = 20*CLHEP::cm;
  if (aper1 < defaultOuterD) // only do if the aperture will fit
    {outerD = std::min(defaultOuterD, outerD);} // better default
  G4double thickness = aperture->beamPipeThickness;
  G4double equatorRadius = outerD - thickness;
  if (equatorRadius <= 0)
    {
      G4cerr << __METHOD_NAME__ << "Combination of outerDiameter and beampipeThickness for"
	     << " element \"" << el->name << "\" produce 0 size cavity" << G4endl;
      exit(1);
    }

  G4double cellLength = 2*CLHEP::c_light / frequency; // half wavelength
  G4double length     = el->l * CLHEP::m;
  G4double nCavities  = length / cellLength;
  G4int nCells = G4int(std::floor(nCavities));
  if (nCells == 0) // protect against long wavelengths or cavities
    {
      nCells = 1;
      cellLength = length;
    }
  
  BDSCavityInfo* defaultCI = new BDSCavityInfo(BDSCavityType::pillbox,
					       BDSMaterials::Instance()->GetMaterial("Copper"),
					       aperture->aper1,
					       thickness,
					       equatorRadius,
					       cellLength*0.5);

  delete aperture;
  return defaultCI;
}

BDSMagnetStrength* BDSComponentFactory::PrepareCavityStrength(Element const* el,
							      G4double currentArcLength) const
{
  BDSMagnetStrength* st = new BDSMagnetStrength();

  G4double chordLength = el->l * CLHEP::m;
  
  if (BDS::IsFinite(el->gradient))
    {(*st)["eField"] = (el->gradient * CLHEP::MeV) / chordLength;}
  else
    {(*st)["eField"] = el->E * CLHEP::volt;}

  (*st)["frequency"] = el->frequency * CLHEP::hertz;

  // phase - construct it so that phase is w.r.t. the centre of the cavity
  // and that it's 0 by default
  G4double frequency = (*st)["frequency"];
  G4double period    = 1. / frequency;
  G4double tOffset   = 0;
  if (BDS::IsFinite(el->tOffset)) // use the one specified
    {tOffset = el->tOffset * CLHEP::ns;}
  else // this gives 0 phase at the middle of cavity
    {tOffset = (currentArcLength + chordLength) / CLHEP::c_light;}

  G4double nPeriods       = tOffset / period;
  // phase is the remainder from total phase / N*2pi, where n is unknown.
  G4double integerPart    = 0;
  G4double fractionalPart = std::modf(nPeriods, &integerPart);
  G4double phaseOffset    = fractionalPart / CLHEP::twopi;

  G4double phase = el->phase * CLHEP::rad;
  if (BDS::IsFinite(el->phase)) // phase specified - use that
    {(*st)["phase"] = phaseOffset + phase;}
  else
    {(*st)["phase"] = phaseOffset;}
  (*st)["equatorRadius"] = 1*CLHEP::m; // to prevent 0 division - updated later on in createRF
  return st;
}

G4String BDSComponentFactory::PrepareColour(Element const* el, const G4String fallback) const
{
  G4String colour = el->colour;
  if (colour == "")
    {colour = fallback;}
  return colour;
}

void BDSComponentFactory::SetFieldDefinitions(Element const* el,
					      BDSAcceleratorComponent* component) const
{
  // Test for a line. And if so apply to each sub-component.
  // TBC - for a sbend split into segments, a BDSLine would be used - how would setting
  // an outer magnetic field work for this??
  G4Transform3D fieldTrans = CreateFieldTransform(element);
  if (BDSLine* line = dynamic_cast<BDSLine*>(component))
    {
      for (auto comp : *line)
	{SetFieldDefinitions(el, comp);}
    }
  if (BDSMagnet* mag = dynamic_cast<BDSMagnet*>(component))
    {
      if (!(el->fieldAll.empty()))
	{
	  G4cerr << "Error: Magnet named \"" << elementName
		 << "\" is a magnet, but has fieldAll defined." << G4endl
		 << "Can only have fieldOuter or fieldVacuum specified." << G4endl;
	  exit(1);
	}
      if (!(el->fieldOuter.empty())) // ie variable isn't ""
	{
	  BDSFieldInfo* info = new BDSFieldInfo(*(BDSFieldFactory::Instance()->GetDefinition(el->fieldOuter)));
	  if (info->ProvideGlobal())
	    {info->SetTransform(info->Transform() * fieldTrans);}
	  mag->SetOuterField(info);
	}
      if (!(el->fieldVacuum.empty()))
	{
	  BDSFieldInfo* info = new BDSFieldInfo(*(BDSFieldFactory::Instance()->GetDefinition(el->fieldVacuum)));
	  if (info->ProvideGlobal())
	    {info->SetTransform(info->Transform() * fieldTrans);}
	  mag->SetVacuumField(info);
	}
    }
  else
    {
      if (!(el->fieldAll.empty()))
	{
	  BDSFieldInfo* info = new BDSFieldInfo(*(BDSFieldFactory::Instance()->GetDefinition(el->fieldAll)));
	  if (info->ProvideGlobal())
	    {info->SetTransform(info->Transform() * fieldTrans);}
	  component->SetField(info);
	}
    }
}

BDSMagnetStrength* BDSComponentFactory::PrepareMagnetStrengthForMultipoles(Element const* el) const
{
  BDSMagnetStrength* st = new BDSMagnetStrength();
  G4double length = el->l;
  // component strength is only normalised by length for thick multipoles
  if (el->type == ElementType::_THINMULT)
    {length = 1;}
  auto kn = el->knl.begin();
  auto ks = el->ksl.begin();
  std::vector<G4String> normKeys = st->NormalComponentKeys();
  std::vector<G4String> skewKeys = st->SkewComponentKeys();
  auto nkey = normKeys.begin();
  auto skey = skewKeys.begin();
  //Separate loops for kn and ks. The length of knl and ksl is determined by the input in the gmad file.
  //A single loop for both kn and ks using only one of their end iterators can end the loop
  //prematurely for the other, potentially missing higher order components.
  for (; kn != el->knl.end(); kn++, nkey++)
    {
      (*st)[*nkey] = (*kn) / length;
    }
  for (; ks != el->ksl.end(); ks++, skey++)
    {
      (*st)[*skey] = (*ks) / length;
    }

  return st;
}

G4double BDSComponentFactory::FieldFromAngle(const G4double angle,
					     const G4double arcLength) const
{
  if (!BDS::IsFinite(angle))
    {return 0;}
  else
    {return brho * angle / arcLength;}
}

G4double BDSComponentFactory::AngleFromField(const G4double field,
					     const G4double arcLength) const
{
  if (!BDS::IsFinite(field))
    {return 0;}
  else
    {return field * arcLength / brho;}
}

void BDSComponentFactory::CalculateAngleAndFieldSBend(Element const* el,
						      G4double&      angle,
						      G4double&      field) const
{
  G4double arcLength = el->l * CLHEP::m;
  if (BDS::IsFinite(el->B) && el->angleSet)
    {// both are specified and should be used - under or overpowered dipole by design
      field = el->B * CLHEP::tesla;
      angle = el->angle * CLHEP::rad;
    }
  else if (BDS::IsFinite(el->B))
    {// only B field - calculate angle
      field = el->B * CLHEP::tesla;
      angle = AngleFromField(field, arcLength); // length is arc length for an sbend
    }
  else
    {// only angle - calculate B field
      angle = el->angle * CLHEP::rad;
      field = FieldFromAngle(angle, arcLength);
    }
}

void BDSComponentFactory::CalculateAngleAndFieldRBend(const Element* el,
						      G4double& arcLength,
						      G4double& chordLength,
						      G4double& field,
						      G4double& angle) const
{
  // 'l' in the element represents the chord length for an rbend - must calculate arc length
  // for the field calculation and the accelerator component.
  chordLength = el->l * CLHEP::m;
  G4double arcLengthLocal = chordLength; // default for no angle
  
  if (BDS::IsFinite(el->B) && el->angleSet)
    {// both are specified and should be used - under or overpowered dipole by design
      field = el->B * CLHEP::tesla;
      // note, angle must be finite for this part to be used so we're protected against
      // infinite bending radius and therefore nan arcLength.
      angle = el->angle * CLHEP::rad;
      G4double bendingRadius = brho / field;

      // protect against bad calculation from 0 angle and finite field
      if (BDS::IsFinite(angle))
        {arcLengthLocal = bendingRadius * angle;}
      else
        {arcLengthLocal = chordLength;}
    }
  else if (BDS::IsFinite(el->B))
    {// only B field - calculate angle
      field = el->B * CLHEP::tesla;
      G4double bendingRadius = brho / field; // in mm as brho already in g4 units
      angle = 2.0*asin(chordLength*0.5 / bendingRadius);
      arcLengthLocal = bendingRadius * angle;
    }
  else
    {// (assume) only angle - calculate B field
      angle = el->angle * CLHEP::rad;
      if (BDS::IsFinite(angle))
	{
	  // sign for bending radius doesn't matter (from angle) as it's only used for arc length.
	  // this is the inverse equation of that in BDSAcceleratorComponent to calculate
	  // the chord length from the arclength and angle.
	  G4double bendingRadius = chordLength * 0.5 / sin(std::abs(angle) * 0.5);
	  arcLengthLocal = bendingRadius * angle;
	  field = brho * angle / std::abs(arcLengthLocal);
        }
      else
	{field = 0;} // 0 angle -> chord length and arc length the same; field 0
    }

  // Ensure positive length despite sign of angle.
  arcLength = std::abs(arcLengthLocal);
}

G4double BDSComponentFactory::BendAngle(const Element* el) const
{
  G4double bendAngle = 0;
  if (el->type == ElementType::_RBEND)
    {
      G4double arcLength = 0, chordLength = 0, field = 0;
      CalculateAngleAndFieldRBend(el, arcLength, chordLength, field, bendAngle);
    }
  else if (el->type == ElementType::_SBEND)
    {
      G4double field = 0; // required by next function.
      CalculateAngleAndFieldSBend(el, bendAngle, field);
    }
  // else the default is 0
  return bendAngle;
}

G4double BDSComponentFactory::OutgoingFaceAngle(const Element* el) const
{
  // note thin multipoles will match the faces of any magnets, but not contain
  // the face angles themselves in the GMAD::Element. this is ok though as
  // detector construction will not give a thin multipole as a previous element
  // - it'll be skipped while looking backwards.
  G4double outgoingFaceAngle = 0;
  G4double bendAngle         = BendAngle(el);

  if (el->type == ElementType::_RBEND)
    {
      // angle is w.r.t. outgoing reference trajectory so rbend face is angled
      // by half the bend angle
      outgoingFaceAngle += 0.5 * bendAngle;
    }
  // for an sbend, the output face or nominally normal to the outgoing
  // reference trajectory - so zero here - only changes with e1/e2.
  // we need angle though to decide which way it goes
  
  // +ve e1/e2 shorten the outside of the bend - so flips with angle
  G4double e2 = el->e2*CLHEP::rad;
  if (BDS::IsFinite(e2))
    {// so if the angle is 0, +1 will be returned
      G4double factor = bendAngle < 0 ? -1 : 1;
      outgoingFaceAngle += factor * e2;
    }
  
  return outgoingFaceAngle;
}

G4double BDSComponentFactory::IncomingFaceAngle(const Element* el) const
{
  // note thin multipoles will match the faces of any magnets, but not contain
  // the face angles themselves in the GMAD::Element. this is ok though as
  // detector construction will not give a thin multipole as a next element
  // - it'll be skipped while looking forwards.
  G4double incomingFaceAngle = 0;
  G4double bendAngle         = BendAngle(el);

  if (el->type == ElementType::_RBEND)
    {
      // angle is w.r.t. outgoing reference trajectory so rbend face is angled
      // by half the bend angle
      incomingFaceAngle += 0.5 * bendAngle;
    }
  // for an sbend, the output face or nominally normal to the outgoing
  // reference trajectory - so zero here - only changes with e1/e2.
  // we need angle though to decide which way it goes

  // +ve e1/e2 shorten the outside of the bend - so flips with angle
  G4double e1 = el->e1*CLHEP::rad;
  if (BDS::IsFinite(e1))
    {// so if the angle is 0, +1 will be returned
      G4double factor = bendAngle < 0 ? -1 : 1;
      incomingFaceAngle += factor * e1;
    }
  
  return incomingFaceAngle;
}
