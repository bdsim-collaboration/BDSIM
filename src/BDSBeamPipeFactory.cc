#include "BDSDebug.hh"

#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipeFactoryCircular.hh"
#include "BDSBeamPipeFactoryElliptical.hh"
#include "BDSBeamPipeFactoryRectangular.hh"
#include "BDSBeamPipeFactoryLHC.hh"
#include "BDSBeamPipeFactoryLHCDetailed.hh"
#include "BDSBeamPipeFactoryRectEllipse.hh"
#include "BDSBeamPipeType.hh"
#include "globals.hh"                        // geant4 globals / types

BDSBeamPipeFactory* BDSBeamPipeFactory::_instance = 0;

BDSBeamPipeFactory* BDSBeamPipeFactory::Instance()
{
  if (_instance == 0)
    {_instance = new BDSBeamPipeFactory();}
  return _instance;
}

BDSBeamPipeFactory::BDSBeamPipeFactory()
{;}

BDSBeamPipeFactoryBase* BDSBeamPipeFactory::GetAppropriateFactory(BDSBeamPipeType type)
{
  switch(type.underlying()){

  case BDSBeamPipeType::circular:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "circular beampipe factory" << G4endl;
#endif
    return BDSBeamPipeFactoryCircular::Instance();
    break;
  case BDSBeamPipeType::elliptical:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "elliptical beampipe factory" << G4endl;
#endif
    return BDSBeamPipeFactoryElliptical::Instance();
    break;
  case BDSBeamPipeType::rectangular:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "rectangular beampipe factory" << G4endl;
#endif
    return BDSBeamPipeFactoryRectangular::Instance();
    break;
  case BDSBeamPipeType::lhc:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "lhc beampipe factory" << G4endl;
#endif
    return BDSBeamPipeFactoryLHC::Instance();
    break;
  case BDSBeamPipeType::lhcdetailed:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "lhc detailed beampipe factory" << G4endl;
#endif
    return BDSBeamPipeFactoryLHCDetailed::Instance();
    break;
  case BDSBeamPipeType::rectellipse:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "rectangular ellipse beampipe factory" << G4endl;
#endif
    return BDSBeamPipeFactoryRectEllipse::Instance();
    break;
  default:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "unknown type \"" << type << "\" - circular beampipe factory by default" << G4endl;
#endif
    return BDSBeamPipeFactoryCircular::Instance();
    break;
  }
}
  
BDSBeamPipe* BDSBeamPipeFactory::CreateBeamPipe(BDSBeamPipeType beamPipeType,
						G4String        name,
						G4double        length,
						G4double        aper1,
						G4double        aper2,
						G4double        aper3,
						G4double        aper4,
						G4Material*     vacuumMaterial,
						G4double        beamPipeThickness,
						G4Material*     beamPipeMaterial
						)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSBeamPipeFactoryBase* factory = GetAppropriateFactory(beamPipeType);
  return factory->CreateBeamPipe(name,length,aper1,aper2,aper3,aper4,
				 vacuumMaterial,beamPipeThickness,beamPipeMaterial);
}

BDSBeamPipe* BDSBeamPipeFactory::CreateBeamPipeAngledIn(BDSBeamPipeType beamPipeType,
							G4String        name,
							G4double        length,
							G4double        angleIn,
							G4double        aper1,
							G4double        aper2,
							G4double        aper3,
							G4double        aper4,
							G4Material*     vacuumMaterial,
							G4double        beamPipeThickness,
							G4Material*     beamPipeMaterial
							)
{
  BDSBeamPipeFactoryBase* factory = GetAppropriateFactory(beamPipeType);
  return factory->CreateBeamPipeAngledIn(name,length,angleIn,aper1,aper2,aper3,aper4,
					 vacuumMaterial,beamPipeThickness,beamPipeMaterial);
}

BDSBeamPipe* BDSBeamPipeFactory::CreateBeamPipeAngledOut(BDSBeamPipeType beamPipeType,
							 G4String        name,
							 G4double        length,
							 G4double        angleOut,
							 G4double        aper1,
							 G4double        aper2,
							 G4double        aper3,
							 G4double        aper4,
							 G4Material*     vacuumMaterial,
							 G4double        beamPipeThickness,
							 G4Material*     beamPipeMaterial
							 )
{
  BDSBeamPipeFactoryBase* factory = GetAppropriateFactory(beamPipeType);
  return factory->CreateBeamPipeAngledOut(name,length,angleOut,aper1,aper2,aper3,aper4,
					  vacuumMaterial,beamPipeThickness,beamPipeMaterial);
}

BDSBeamPipe* BDSBeamPipeFactory::CreateBeamPipeAngledInOut(BDSBeamPipeType beamPipeType,
							   G4String        name,
							   G4double        length,
							   G4double        angleIn,
							   G4double        angleOut,
							   G4double        aper1,
							   G4double        aper2,
							   G4double        aper3,
							   G4double        aper4,
							   G4Material*     vacuumMaterial,
							   G4double        beamPipeThickness,
							   G4Material*     beamPipeMaterial
							   )
{
  BDSBeamPipeFactoryBase* factory = GetAppropriateFactory(beamPipeType);
  return factory->CreateBeamPipeAngledInOut(name,length,angleIn,angleOut,aper1,aper2,aper3,aper4,
					    vacuumMaterial,beamPipeThickness,beamPipeMaterial);
}
