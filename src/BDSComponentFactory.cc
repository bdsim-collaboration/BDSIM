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
							      Element const* nextElementIn)
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

  BDSAcceleratorComponent* component = nullptr;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " - creating " << G4endl;
  element->print();
#endif
  switch(element->type){
  case ElementType::_DRIFT:
    component = CreateDrift(angleIn, angleOut); break;
  case ElementType::_RF:
    component = CreateRF(); break;
  case ElementType::_SBEND:
    component = CreateSBend(); break;
  case ElementType::_RBEND:
    component = CreateRBend(); break;
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
			    teleporterDelta));
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
      G4cerr << __METHOD_NAME__ << "Drift \"" << element->name
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

  return (new BDSDrift( element->name,
			length,
			beamPipeInfo));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateRF()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSIntegratorType intType = integratorSet->Integrator(BDSFieldType::rfcavity);
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

  PoleFaceRotationsNotTooLarge(element);

  // don't check here on whether the possibly next / previous sbend will clash with
  // pole face angles - let that be checked after element construction in the beamline

  BDSMagnetStrength* st = new BDSMagnetStrength();
  std::pair<G4double,G4double> angleAndField = CalculateAngleAndField(element);
  (*st)["angle"]  = angleAndField.first;
  (*st)["field"]  = angleAndField.second;
  (*st)["length"] = element->l * CLHEP::m;

  // Quadrupole component
  if (BDS::IsFinite(element->k1))
    {(*st)["k1"] = element->k1 / CLHEP::m2;}

#ifdef BDSDEBUG
  G4cout << "Angle " << (*st)["angle"] << G4endl;
  G4cout << "Field " << (*st)["field"] << G4endl;
#endif
  
  auto sBendLine = BDS::BuildSBendLine(element, st, brho, integratorSet, charge);
  
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
  G4double arcLength   = 0, chordLength = 0, field = 0, angle = 0;
  CalculateAngleAndFieldRBend(element, arcLength, chordLength, field, angle);
  
  (*st)["angle"]  = angle;
  (*st)["field"]  = field;
  (*st)["length"] = arcLength;

  // Check the faces won't overlap due to too strong an angle with too short a magnet
  G4double outerDiameter = PrepareOuterDiameter(element);
  CheckBendLengthAngleWidthCombo(arcLength, (*st)["angle"], outerDiameter, element->name);

  // Quadrupole component
  if (BDS::IsFinite(element->k1))
    {(*st)["k1"] = element->k1 / CLHEP::m2;}

  BDSLine* rbendline = BDS::BuildRBendLine(element,
					   prevElement,
					   nextElement,
					   brho,
					   st,
					   integratorSet,
					   charge);
  return rbendline;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateKicker(G4bool isVertical)
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  auto angleAndField = CalculateAngleAndField(element);
  // MADX definition is that +ve hkick (here angle) increases p_x, corresponding
  // to deflection in +ve x, which is opposite to the convention of bends.
  // Hence -ve factor here.
  (*st)["angle"] = -angleAndField.first;
  (*st)["field"] = -angleAndField.second;
  
  G4Transform3D fieldRotation = G4Transform3D();  
  BDSMagnetType t = BDSMagnetType::hkicker;
  if (isVertical)
    {
      t = BDSMagnetType::vkicker;
      fieldRotation = G4RotateZ3D(-CLHEP::halfpi);
    }
  
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::dipole,
					       brho,
					       BDSIntegratorType::g4classicalrk4,
					       st,
					       true,
					       fieldRotation);

  G4bool yokeOnLeft = YokeOnLeft(element, st);
  
  return new BDSMagnet(t,
		       element->name,
		       element->l*CLHEP::m,
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
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::multipole,
					       brho,
					       intType,
					       st);
  
  BDSMagnet* thinMultipole =  new BDSMagnet(BDSMagnetType::thinmultipole,
					    element->name,
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
  
  return (new BDSElement(element->name,
			 element->l * CLHEP::m,
			 element->outerDiameter * CLHEP::m,
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
  BDSIntegratorType intType = integratorSet->Integrator(BDSFieldType::muonspoiler);
  BDSFieldInfo* outerField = new BDSFieldInfo(BDSFieldType::muonspoiler,
					      brho,
					      intType,
					      st);
  BDSFieldInfo* vacuumField = new BDSFieldInfo();

  return new BDSMagnet(BDSMagnetType::muonspoiler,
		       element->name,
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
  BDSShield* shield = new BDSShield(element->name,
				    element->l*CLHEP::m,
				    element->outerDiameter*CLHEP::m,
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
      ss << element->layerMaterials.size() << " and " << element->layerThicknesses.size();
      G4Exception(ss.str().c_str(), "-1", FatalException, "");
    }
  if( (element->layerThicknesses.size() != element->layerIsSampler.size()) && ( element->layerIsSampler.size() !=0 ))
    {
      std::stringstream ss;
      ss << "Material and isSampler lists are of unequal size.";
      ss << element->layerMaterials.size() << " and " << element->layerIsSampler.size();
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
      G4cout << __METHOD_NAME__ << " - screen layer: thickness: " 
	     << *(itt) << ", material "  << (*itm)
	     <<	", isSampler: "  << (*itIsSampler) << G4endl;
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
  return (new BDSAwakeScintillatorScreen(element->name,
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
      
      awakeField = new BDSFieldInfo(BDSFieldType::dipole,
				    brho,
				    BDSIntegratorType::g4nystromrk4,
				    awakeStrength);
    }
  else
    {awakeField = BDSFieldFactory::Instance()->GetDefinition(element->fieldAll);}
  return (new BDSAwakeSpectrometer(element->name,
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

BDSMagnet* BDSComponentFactory::CreateMagnet(BDSMagnetStrength* st,
					     BDSFieldType fieldType,
					     BDSMagnetType magnetType,
					     G4double angle) const
{
  BDSIntegratorType intType = integratorSet->Integrator(fieldType);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(fieldType,
					       brho,
					       intType,
					       st);

  return new BDSMagnet(magnetType,
		       element->name,
		       element->l * CLHEP::m,
		       PrepareBeamPipeInfo(element),
		       PrepareMagnetOuterInfo(element, st),
		       vacuumField,
		       angle);
}

G4bool BDSComponentFactory::HasSufficientMinimumLength(Element const* element)
{
  if(element->l*CLHEP::m < 1e-7)
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

  // yoke direction
  info->yokeOnLeft = yokeOnLeft;

  return info;
}

G4double BDSComponentFactory::PrepareOuterDiameter(Element const* element)
{
  G4double outerDiameter = element->outerDiameter*CLHEP::m;
  if (outerDiameter < 1e-6)
    {//outerDiameter not set - use global option as default
      outerDiameter = BDSGlobalConstants::Instance()->OuterDiameter();
    }
  return outerDiameter;
}

BDSBeamPipeInfo* BDSComponentFactory::PrepareBeamPipeInfo(Element const* element,
							  const G4ThreeVector inputFaceNormal,
							  const G4ThreeVector outputFaceNormal)
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
							  const G4double angleOut)
{
  auto faces = BDS::CalculateFaces(angleIn, angleOut);
  BDSBeamPipeInfo* info = PrepareBeamPipeInfo(element, faces.first, faces.second);
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
  // Test for a line. And if so apply to each sub-component.
  // TBC - for a sbend split into segments, a BDSLine would be used - how would setting
  // an outer magnetic field work for this??
  if (BDSLine* line = dynamic_cast<BDSLine*>(component))
    {
      for (auto comp : *line)
	{SetFieldDefinitions(element, comp);}
    }
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

BDSMagnetStrength* BDSComponentFactory::PrepareMagnetStrengthForMultipoles(Element const* element) const
{
  BDSMagnetStrength* st = new BDSMagnetStrength();
  G4double length = element->l;
  // component strength is only normalised by length for thick multipoles
  if (element->type == ElementType::_THINMULT)
    {length = 1;}
  auto kn = element->knl.begin();
  auto ks = element->ksl.begin();
  std::vector<G4String> normKeys = st->NormalComponentKeys();
  std::vector<G4String> skewKeys = st->SkewComponentKeys();
  auto nkey = normKeys.begin();
  auto skey = skewKeys.begin();
  for (; kn != element->knl.end(); kn++, ks++, nkey++, skey++)
    {
      (*st)[*nkey] = (*kn) / length;
      (*st)[*skey] = (*ks) / length;
    }
  return st;
}

std::pair<G4double,G4double> BDSComponentFactory::CalculateAngleAndField(Element const* element) const
{  
  G4double angle  = 0;
  G4double field  = 0;  
  G4double length = element->l * CLHEP::m;
  G4double ffact  = BDSGlobalConstants::Instance()->FFact();
  
  if (BDS::IsFinite(element->B) && BDS::IsFinite(element->angle))
    {// both are specified and should be used - under or overpowered dipole by design
      field = element->B * CLHEP::tesla;
      angle = element->angle * CLHEP::rad;
    }
  else if (BDS::IsFinite(element->B))
    {// only B field - calculate angle
      field = element->B * CLHEP::tesla;
      angle = charge * ffact * field * length / brho ;
    }
  else
    {// only angle - calculate B field
      angle = element->angle * CLHEP::rad;
      field = brho * angle / length / charge * ffact;
    }
  
  return std::make_pair(angle,field);
}

void BDSComponentFactory::CalculateAngleAndFieldRBend(const Element* element,
						      G4double& arcLength,
						      G4double& chordLength,
						      G4double& field,
						      G4double& angle) const
{
  // 'l' in the element represents the chord length for an rbend - must calculate arc length
  // for the field calculation and the accelerator component.
  chordLength = element->l * CLHEP::m;
  G4double arcLengthLocal = chordLength;
  G4double ffact = BDSGlobalConstants::Instance()->FFact();
  
  if (BDS::IsFinite(element->B) && BDS::IsFinite(element->angle))
    {// both are specified and should be used - under or overpowered dipole by design
      field = element->B * CLHEP::tesla;
      // note, angle must be finite for this part to be used so we're protected against
      // infinite bending radius and therefore nan arcLength.
      angle = element->angle * CLHEP::rad;
      G4double bendingRadius = brho / field;
      arcLengthLocal = bendingRadius * angle;
    }
  else if (BDS::IsFinite(element->B))
    {// only B field - calculate angle
      field = element->B * CLHEP::tesla;
      G4double bendingRadius = brho / field; // in mm as brho already in g4 units
      angle = charge * ffact * 2.0*asin(chordLength*0.5 / bendingRadius);
      arcLengthLocal = bendingRadius * angle;
    }
  else
    {// (assume) only angle - calculate B field
      angle = element->angle * CLHEP::rad;
      if (BDS::IsFinite(angle))
	{
	  // sign for bending radius doesn't matter (from angle) as it's only used for arc length.
	  G4double bendingRadius = chordLength * 0.5 / sin(std::abs(angle) * 0.5);
	  arcLengthLocal = bendingRadius * angle;
	  field = brho * angle / std::abs(arcLengthLocal) / charge * ffact;
        }
      else
	{field = 0;} // 0 angle -> chord length and arc length the same; field 0
    }

  // Ensure positive length despite sign of angle or charge.
  arcLength = std::abs(arcLengthLocal);
}

G4double BDSComponentFactory::BendAngle(const Element* element) const
{
  G4double bendAngle = 0;
  if (element->type == ElementType::_RBEND)
    {
      G4double arcLength = 0, chordLength = 0, field = 0;
      CalculateAngleAndFieldRBend(element, arcLength, chordLength, field, bendAngle);
    }
  else if (element->type == ElementType::_SBEND)
    {
      auto angleAndField = CalculateAngleAndField(element);
      bendAngle = angleAndField.first;
    }
  return bendAngle;
}

G4double BDSComponentFactory::OutgoingFaceAngle(const Element* element) const
{
  // note thin multipoles will match the faces of any magnets, but not contain
  // the face angles themselves in the GMAD::Element. this is ok though as
  // detector construction will not give a thin multipole as a previous element
  // - it'll be skipped while looking backwards.
  G4double outgoingFaceAngle = 0;
  G4double bendAngle         = BendAngle(element);

  if (element->type == ElementType::_RBEND)
    {
      // angle is w.r.t. outgoing reference trajectory so rbend face is angled
      // by half the bend angle
      outgoingFaceAngle += 0.5 * bendAngle;
    }
  // for an sbend, the output face or nominally normal to the outgoing
  // reference trajectory - so zero here - only changes with e1/e2.
  // we need angle though to decide which way it goes
  
  // +ve e1/e2 shorten the outside of the bend - so flips with angle
  G4double e2 = element->e2*CLHEP::rad;
  if (BDS::IsFinite(e2))
    {// so if the angle is 0, +1 will be returned
      G4double factor = bendAngle < 0 ? -1 : 1;
      outgoingFaceAngle += factor * e2;
    }
  
  return outgoingFaceAngle;
}

G4double BDSComponentFactory::IncomingFaceAngle(const Element* element) const
{
  // note thin multipoles will match the faces of any magnets, but not contain
  // the face angles themselves in the GMAD::Element. this is ok though as
  // detector construction will not give a thin multipole as a next element
  // - it'll be skipped while looking forwards.
  G4double incomingFaceAngle = 0;
  G4double bendAngle         = BendAngle(element);

  if (element->type == ElementType::_RBEND)
    {
      // angle is w.r.t. outgoing reference trajectory so rbend face is angled
      // by half the bend angle
      incomingFaceAngle += 0.5 * bendAngle;
    }
  // for an sbend, the output face or nominally normal to the outgoing
  // reference trajectory - so zero here - only changes with e1/e2.
  // we need angle though to decide which way it goes

  // +ve e1/e2 shorten the outside of the bend - so flips with angle
  G4double e1 = element->e1*CLHEP::rad;
  if (BDS::IsFinite(e1))
    {// so if the angle is 0, +1 will be returned
      G4double factor = bendAngle < 0 ? -1 : 1;
      incomingFaceAngle += factor * e1;
    }
  
  return incomingFaceAngle;
}
