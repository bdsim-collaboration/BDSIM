#include "BDSDebug.hh"
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

BDSMagnetOuterFactory* BDSMagnetOuterFactory::_instance = 0;

BDSMagnetOuterFactory* BDSMagnetOuterFactory::Instance()
{
  if (_instance == 0)
    {_instance = new BDSMagnetOuterFactory();}
  return _instance;
}

BDSMagnetOuterFactory::BDSMagnetOuterFactory()
{;}

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

BDSGeometryComponent* BDSMagnetOuterFactory::CreateSectorBend(BDSMagnetGeometryType magnetType,
							      G4String      name,
							      G4double      length,
							      BDSBeamPipe*  beamPipe,
							      G4double      boxSize,
							      G4double      angle,
							      G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateSectorBend(name, length, beamPipe, boxSize, angle, outerMaterial);
}

BDSGeometryComponent* BDSMagnetOuterFactory::CreateRectangularBend(BDSMagnetGeometryType magnetType,
								   G4String      name,
								   G4double      length,
								   BDSBeamPipe*  beamPipe,
								   G4double      boxSize,
								   G4double      angle,
								   G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateRectangularBend(name, length, beamPipe, boxSize, angle, outerMaterial);
}
  

BDSGeometryComponent* BDSMagnetOuterFactory::CreateQuadrupole(BDSMagnetGeometryType magnetType,
							      G4String      name,
							      G4double      length,
							      BDSBeamPipe*  beamPipe,
							      G4double      boxSize,
							      G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateQuadrupole(name, length, beamPipe, boxSize, outerMaterial);
}

BDSGeometryComponent* BDSMagnetOuterFactory::CreateSextupole(BDSMagnetGeometryType magnetType,
							     G4String      name,
							     G4double      length,
							     BDSBeamPipe*  beamPipe,
							     G4double      boxSize,
							     G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateSextupole(name, length, beamPipe, boxSize, outerMaterial);
}

BDSGeometryComponent* BDSMagnetOuterFactory::CreateOctupole(BDSMagnetGeometryType magnetType,
							    G4String      name,
							    G4double      length,
							    BDSBeamPipe*  beamPipe,
							    G4double      boxSize,
							    G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateOctupole(name, length, beamPipe, boxSize, outerMaterial);
}

BDSGeometryComponent* BDSMagnetOuterFactory::CreateDecapole(BDSMagnetGeometryType magnetType,
							    G4String      name,
							    G4double      length,
							    BDSBeamPipe*  beamPipe,
							    G4double      boxSize,
							    G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateDecapole(name, length, beamPipe, boxSize, outerMaterial);
}

BDSGeometryComponent* BDSMagnetOuterFactory::CreateSolenoid(BDSMagnetGeometryType magnetType,
							    G4String      name,
							    G4double      length,
							    BDSBeamPipe*  beamPipe,
							    G4double      boxSize,
							    G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateSolenoid(name, length, beamPipe, boxSize, outerMaterial);
}

BDSGeometryComponent* BDSMagnetOuterFactory::CreateMultipole(BDSMagnetGeometryType magnetType,
							     G4String      name,
							     G4double      length,
							     BDSBeamPipe*  beamPipe,
							     G4double      boxSize,
							     G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateMultipole(name, length, beamPipe, boxSize, outerMaterial);
}

BDSGeometryComponent* BDSMagnetOuterFactory::CreateRfCavity(BDSMagnetGeometryType magnetType,
							    G4String      name,
							    G4double      length,
							    BDSBeamPipe*  beamPipe,
							    G4double      boxSize,
							    G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateRfCavity(name, length, beamPipe, boxSize, outerMaterial);
}

BDSGeometryComponent* BDSMagnetOuterFactory::CreateMuSpoiler(BDSMagnetGeometryType magnetType,
						G4String      name,
						G4double      length,
						BDSBeamPipe*  beamPipe,
						G4double      boxSize,
							     G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateMuSpoiler(name, length, beamPipe, boxSize, outerMaterial);
}

BDSGeometryComponent* BDSMagnetOuterFactory::CreateKicker(BDSMagnetGeometryType magnetType,
							  G4String      name,
							  G4double      length,
							  BDSBeamPipe*  beamPipe,
							  G4double      boxSize,
							  G4bool        vertical,
							  G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateKicker(name, length, beamPipe, boxSize, vertical, outerMaterial);
}
