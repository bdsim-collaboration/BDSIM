#include "BDSDebug.hh"

#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipeFactoryCircular.hh"
//#include "BDSBeamPipeFactoryElliptical.hh"
//#include "BDSBeamPipeFactoryRectangular.hh"
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

BDSBeamPipeFactoryBase* BDSBeamPipeFactory::GetAppropriateFactory(BDSBeamPipeType typeIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  switch(typeIn.underlying()){

  case BDSBeamPipeType::circular:
    return BDSBeamPipeFactoryCircular::Instance();
    break;
  case BDSBeamPipeType::elliptical:
    //return BDSBeamPipeFactoryElliptical::Instance();
    return BDSBeamPipeFactoryCircular::Instance();
    break;
  case BDSBeamPipeType::rectangular:
    //return BDSBeamPipeFactoryRectangular::Instance();
    return BDSBeamPipeFactoryCircular::Instance();
    break;
  default:
    return BDSBeamPipeFactoryCircular::Instance();
    break;
  }
}
  
BDSBeamPipe* BDSBeamPipeFactory::CreateBeamPipe(BDSBeamPipeType beamPipeTypeIn,
						G4String        nameIn,
						G4double        lengthIn,
						G4Material*     vacuumMaterialIn,
						G4double        beamPipeThicknessIn,
						G4Material*     beamPipeMaterialIn,
						G4double        aper1,
						G4double        aper2,
						G4double        aper3,
						G4double        aper4
						)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSBeamPipeFactoryBase* factory = GetAppropriateFactory(beamPipeTypeIn);
  return factory->CreateBeamPipe(nameIn,lengthIn,vacuumMaterialIn,beamPipeThicknessIn,
				 beamPipeMaterialIn,aper1,aper2,aper3,aper4);
}

BDSBeamPipe* BDSBeamPipeFactory::CreateBeamPipeAngledIn(BDSBeamPipeType beamPipeTypeIn,
							G4String        nameIn,
							G4double        lengthIn,
							G4double        angleInIn,
							G4Material*     vacuumMaterialIn,
							G4double        beamPipeThicknessIn,
							G4Material*     beamPipeMaterialIn,
							G4double        aper1,
							G4double        aper2,
							G4double        aper3,
							G4double        aper4
							)
{
  BDSBeamPipeFactoryBase* factory = GetAppropriateFactory(beamPipeTypeIn);
  return factory->CreateBeamPipeAngledIn(nameIn,lengthIn,angleInIn,vacuumMaterialIn,
					 beamPipeThicknessIn,beamPipeMaterialIn,
					 aper1,aper2,aper3,aper4);
}

BDSBeamPipe* BDSBeamPipeFactory::CreateBeamPipeAngledOut(BDSBeamPipeType beamPipeTypeIn,
							 G4String        nameIn,
							 G4double        lengthIn,
							 G4double        angleOutIn,
							 G4Material*     vacuumMaterialIn,
							 G4double        beamPipeThicknessIn,
							 G4Material*     beamPipeMaterialIn,
							 G4double        aper1,
							 G4double        aper2,
							 G4double        aper3,
							 G4double        aper4
							 )
{
  BDSBeamPipeFactoryBase* factory = GetAppropriateFactory(beamPipeTypeIn);
  return factory->CreateBeamPipeAngledOut(nameIn,lengthIn,angleOutIn,vacuumMaterialIn,
					  beamPipeThicknessIn,beamPipeMaterialIn,
					  aper1,aper2,aper3,aper4);
}

BDSBeamPipe* BDSBeamPipeFactory::CreateBeamPipeAngledInOut(BDSBeamPipeType beamPipeTypeIn,
							   G4String        nameIn,
							   G4double        lengthIn,
							   G4double        angleInIn,
							   G4double        angleOutIn,
							   G4Material*     vacuumMaterialIn,
							   G4double        beamPipeThicknessIn,
							   G4Material*     beamPipeMaterialIn,
							   G4double        aper1,
							   G4double        aper2,
							   G4double        aper3,
							   G4double        aper4
							   )
{
  BDSBeamPipeFactoryBase* factory = GetAppropriateFactory(beamPipeTypeIn);
  return factory->CreateBeamPipeAngledInOut(nameIn,lengthIn,angleInIn,angleOutIn,
					    vacuumMaterialIn,beamPipeThicknessIn,
					    beamPipeMaterialIn,aper1,aper2,aper3,aper4);
}
