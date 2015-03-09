#include "BDSDebug.hh"
#include "BDSMagnetOuterFactory.hh"
#include "BDSMagnetOuterFactoryBase.hh"
#include "BDSMagnetOuterFactoryCylindrical.hh"
//#include "BDSMagnetOuterFactoryNC.hh"
//#include "BDSMagnetOuterFactoryLHC.hh"
#include "BDSMagnetType.hh"

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

BDSMagnetOuterFactoryBase* BDSMagnetOuterFactory::GetAppropriateFactory(BDSMagnetType magnetTypeIn)
{
  switch(magnetTypeIn.underlying()){

  case BDSMagnetType::cylindrical:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << " cylindrical magnet factory" << G4endl;
#endif
    return BDSMagnetOuterFactoryCylindrical::Instance();
    break;
  case BDSMagnetType::normalconducting:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << " normal conducting magnet factory" << G4endl;
#endif
    //return BDSMagnetOuterFactoryNC::Instance();
    return BDSMagnetOuterFactoryCylindrical::Instance();
    break;
  case BDSMagnetType::lhc:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << " LHC magnet factory" << G4endl;
#endif
    //return BDSMagnetOuterFactoryLHC::Instance();
    return BDSMagnetOuterFactoryCylindrical::Instance();
    break;
  default:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "unknown type \"" << magnetTypeIn
	   << "\" - cylindrical factory by default" << G4endl;
#endif
    return BDSMagnetOuterFactoryCylindrical::Instance();
    break;
  }
}

BDSGeometryComponent* BDSMagnetOuterFactory::CreateSectorBend(BDSMagnetType magnetType,
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

BDSGeometryComponent* BDSMagnetOuterFactory::CreateRectangularBend(BDSMagnetType magnetType,
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
  

BDSGeometryComponent* BDSMagnetOuterFactory::CreateQuadrupole(BDSMagnetType magnetType,
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

BDSGeometryComponent* BDSMagnetOuterFactory::CreateSextupole(BDSMagnetType magnetType,
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

BDSGeometryComponent* BDSMagnetOuterFactory::CreateOctupole(BDSMagnetType magnetType,
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

BDSGeometryComponent* BDSMagnetOuterFactory::CreateDecapole(BDSMagnetType magnetType,
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

BDSGeometryComponent* BDSMagnetOuterFactory::CreateSolenoid(BDSMagnetType magnetType,
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

BDSGeometryComponent* BDSMagnetOuterFactory::CreateMultipole(BDSMagnetType magnetType,
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

BDSGeometryComponent* BDSMagnetOuterFactory::CreateRfCavity(BDSMagnetType magnetType,
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

BDSGeometryComponent* BDSMagnetOuterFactory::CreateMuSpoiler(BDSMagnetType magnetType,
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

BDSGeometryComponent* BDSMagnetOuterFactory::CreateKicker(BDSMagnetType magnetType,
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
