#include "BDSDebug.hh"
#include "BDSMagnetOuter.hh"
#include "BDSMagnetOuterFactory.hh"
#include "BDSMagnetOuterFactoryBase.hh"
#include "BDSMagnetOuterFactoryCylindrical.hh"
#include "BDSMagnetOuterFactoryPolesCircular.hh"
#include "BDSMagnetOuterFactoryPolesFacet.hh"
#include "BDSMagnetOuterFactoryPolesFacetCrop.hh"
#include "BDSMagnetOuterFactoryPolesSquare.hh"
#include "BDSMagnetOuterFactoryLHCLeft.hh"
#include "BDSMagnetOuterFactoryLHCRight.hh"
#include "BDSMagnetOuterFactoryNone.hh"
#include "BDSMagnetGeometryType.hh"

#include "globals.hh"                        // geant4 globals / types

BDSMagnetOuterFactory* BDSMagnetOuterFactory::_instance = nullptr;

BDSMagnetOuterFactory* BDSMagnetOuterFactory::Instance()
{
  if (_instance == nullptr)
    {_instance = new BDSMagnetOuterFactory();}
  return _instance;
}

BDSMagnetOuterFactory::BDSMagnetOuterFactory()
{;}

BDSMagnetOuterFactory::~BDSMagnetOuterFactory()
{
  _instance = nullptr;
}

BDSMagnetOuterFactoryBase* BDSMagnetOuterFactory::GetAppropriateFactory(BDSMagnetGeometryType magnetTypeIn)
{
  switch(magnetTypeIn.underlying()){
    
  case BDSMagnetGeometryType::none:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "'none' magnet factory (no outer geometry)" << G4endl;
#endif
    return BDSMagnetOuterFactoryNone::Instance();
    break;
  case BDSMagnetGeometryType::cylindrical:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "cylindrical magnet factory" << G4endl;
#endif
    return BDSMagnetOuterFactoryCylindrical::Instance();
    break;
  case BDSMagnetGeometryType::polescircular:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "poles with circular yoke factory" << G4endl;
#endif
    return BDSMagnetOuterFactoryPolesCircular::Instance();
    break;
  case BDSMagnetGeometryType::polessquare:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "poles with square yoke factory" << G4endl;
#endif
    return BDSMagnetOuterFactoryPolesSquare::Instance();
    break;
  case BDSMagnetGeometryType::polesfacet:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "poles with faceted yoke factory" << G4endl;
#endif
    return BDSMagnetOuterFactoryPolesFacet::Instance();
    break;
  case BDSMagnetGeometryType::polesfacetcrop:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "poles with faceted and cropped yoke factory" << G4endl;
#endif
    return BDSMagnetOuterFactoryPolesFacetCrop::Instance();
    break;
  case BDSMagnetGeometryType::lhcleft:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "LHC magnet factory - with left offset" << G4endl;
#endif
    return BDSMagnetOuterFactoryLHCLeft::Instance();
    break;
  case BDSMagnetGeometryType::lhcright:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "LHC magnet factory - with right offset" << G4endl;
#endif
    return BDSMagnetOuterFactoryLHCRight::Instance();
    break;
  default:
    G4cerr << __METHOD_NAME__ << "unknown type \"" << magnetTypeIn << G4endl;
    exit(1);
    break;
  }
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateMagnetOuter(BDSMagnetType       magnetType,
							 BDSMagnetOuterInfo* outerInfo,
							 G4double            outerLength,
							 G4double            chordLength,
							 BDSBeamPipe*        beampipe)
{
  BDSMagnetOuter* outer = nullptr;

  G4String name                         = outerInfo->name;
  G4double outerDiameter                = outerInfo->outerDiameter;
  G4Material* outerMaterial             = outerInfo->outerMaterial;
  BDSMagnetGeometryType geometryType    = outerInfo->geometryType;
  
  switch(magnetType.underlying())
    {
    case BDSMagnetType::decapole:
      outer = CreateDecapole(geometryType,name,outerLength,beampipe,
			     outerDiameter,chordLength,outerMaterial);
      break;
    case BDSMagnetType::vkicker:
      outer = CreateKicker(geometryType,name,outerLength,beampipe,
			   outerDiameter,chordLength,true,outerMaterial);
      break;
    case BDSMagnetType::hkicker:
      outer = CreateKicker(geometryType,name,outerLength,beampipe,
			   outerDiameter,chordLength,false,outerMaterial);
      break;
    case BDSMagnetType::muonspoiler:
      outer = CreateMuSpoiler(geometryType,name,outerLength,beampipe,
			      outerDiameter,chordLength,outerMaterial);
      break;
    case BDSMagnetType::octupole:
      outer = CreateOctupole(geometryType,name,outerLength,beampipe,
			     outerDiameter,chordLength,outerMaterial);
      break;
    case BDSMagnetType::quadrupole:
      outer = CreateQuadrupole(geometryType,name,outerLength,beampipe,
			       outerDiameter,chordLength,outerMaterial);
      break;
    case BDSMagnetType::rfcavity:
      outer = CreateRfCavity(geometryType,name,outerLength,beampipe,
			     outerDiameter,chordLength,outerMaterial);
      break;
    case BDSMagnetType::sectorbend:
      outer = CreateSectorBend(geometryType,name,outerLength,beampipe,
			       outerDiameter,chordLength,outerInfo->angleIn,
			       outerInfo->angleOut,outerMaterial);
      break;
    case BDSMagnetType::sextupole:
      outer = CreateSextupole(geometryType,name,outerLength,beampipe,
			      outerDiameter,chordLength,outerMaterial);
      break;
    case BDSMagnetType::solenoid:
      outer = CreateSolenoid(geometryType,name,outerLength,beampipe,
			     outerDiameter,chordLength,outerMaterial);
      break;
    case BDSMagnetType::multipole:
      outer = CreateMultipole(geometryType,name,outerLength,beampipe,
			      outerDiameter,chordLength,outerMaterial);
      break;
    default:
      G4cout << __METHOD_NAME__ << "unknown magnet type - no outer volume built" << G4endl;
      break;
    }
  return outer;
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateSectorBend(BDSMagnetGeometryType magnetType,
							G4String      name,
							G4double      length,
							BDSBeamPipe*  beamPipe,
							G4double      outerDiameter,
							G4double      containerLength,
							G4double      angleIn,
							G4double      angleOut,
							G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateSectorBend(name, length, beamPipe, outerDiameter, containerLength,
				   angleIn, angleOut, outerMaterial);
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateRectangularBend(BDSMagnetGeometryType magnetType,
							     G4String      name,
							     G4double      length,
							     BDSBeamPipe*  beamPipe,
							     G4double      outerDiameter,
							     G4double      containerDiameter,
							     G4double      containerLength,
							     G4double      angleIn,
							     G4double      angleOut,
							     G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateRectangularBend(name, length, beamPipe, outerDiameter, containerDiameter,
					containerLength, angleIn, angleOut, outerMaterial);
}
  

BDSMagnetOuter* BDSMagnetOuterFactory::CreateQuadrupole(BDSMagnetGeometryType magnetType,
							G4String      name,
							G4double      length,
							BDSBeamPipe*  beamPipe,
							G4double      outerDiameter,
							G4double      containerLength,
							G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateQuadrupole(name, length, beamPipe, outerDiameter, containerLength, outerMaterial);
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateSextupole(BDSMagnetGeometryType magnetType,
						       G4String      name,
						       G4double      length,
						       BDSBeamPipe*  beamPipe,
						       G4double      outerDiameter,
						       G4double      containerLength,
						       G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateSextupole(name, length, beamPipe, outerDiameter, containerLength, outerMaterial);
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateOctupole(BDSMagnetGeometryType magnetType,
						      G4String      name,
						      G4double      length,
						      BDSBeamPipe*  beamPipe,
						      G4double      outerDiameter,
						      G4double      containerLength,
						      G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateOctupole(name, length, beamPipe, outerDiameter, containerLength, outerMaterial);
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateDecapole(BDSMagnetGeometryType magnetType,
						      G4String      name,
						      G4double      length,
						      BDSBeamPipe*  beamPipe,
						      G4double      outerDiameter,
						      G4double      containerLength,
						      G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateDecapole(name, length, beamPipe, outerDiameter, containerLength, outerMaterial);
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateSolenoid(BDSMagnetGeometryType magnetType,
						      G4String      name,
						      G4double      length,
						      BDSBeamPipe*  beamPipe,
						      G4double      outerDiameter,
						      G4double      containerLength,
						      G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateSolenoid(name, length, beamPipe, outerDiameter, containerLength, outerMaterial);
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateMultipole(BDSMagnetGeometryType magnetType,
						       G4String      name,
						       G4double      length,
						       BDSBeamPipe*  beamPipe,
						       G4double      outerDiameter,
						       G4double      containerLength,
						       G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateMultipole(name, length, beamPipe, outerDiameter, containerLength, outerMaterial);
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateRfCavity(BDSMagnetGeometryType magnetType,
						      G4String      name,
						      G4double      length,
						      BDSBeamPipe*  beamPipe,
						      G4double      outerDiameter,
						      G4double      containerLength,
						      G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateRfCavity(name, length, beamPipe, outerDiameter, containerLength, outerMaterial);
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateMuSpoiler(BDSMagnetGeometryType magnetType,
						       G4String      name,
						       G4double      length,
						       BDSBeamPipe*  beamPipe,
						       G4double      outerDiameter,
						       G4double      containerLength,
						       G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateMuSpoiler(name, length, beamPipe, outerDiameter, containerLength, outerMaterial);
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateKicker(BDSMagnetGeometryType magnetType,
						    G4String      name,
						    G4double      length,
						    BDSBeamPipe*  beamPipe,
						    G4double      outerDiameter,
						    G4double      containerLength,
						    G4bool        vertical,
						    G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateKicker(name, length, beamPipe, outerDiameter, containerLength, vertical, outerMaterial);
}
