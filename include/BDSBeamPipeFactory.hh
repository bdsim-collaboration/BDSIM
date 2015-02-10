#ifndef BDSBEAMPIPEFACTORY_H
#define BDSBEAMPIPEFACTORY_H

#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipeType.hh"

#include "globals.hh"           // geant4 globals / types
#include "G4Material.hh"

class BDSBeamPipeFactory
{
public:
  static BDSBeamPipeFactory* Instance(); /// singleon pattern
  
  ~BDSBeamPipeFactory();
  
  BDSBeamPipe* CreateBeamPipe(BDSBeamPipeType beamPipeTypeIn,            // aperture type
			      G4String        nameIn,                    // name
			      G4double        lengthIn,                  // length [mm]
			      G4Material*     vacuumMaterialIn = NULL,   // vacuum material
			      G4double        beamPipeThicknessIn = 0,   // beampipe thickness [mm]
			      G4Material*     beamPipeMaterialIn = NULL, // beampipe material
			      G4double        aper1 = 0,                 // aperture parameter 1
			      G4double        aper2 = 0,                 // aperture parameter 2
			      G4double        aper3 = 0,                 // aperture parameter 3
			      G4double        aper4 = 0                  // aperture parameter 4
			      );

  BDSBeamPipe* CreateBeamPipeAngledIn(   BDSBeamPipeType beamPipeTypeIn,
				         G4String        nameIn,
				         G4double        lengthIn,
				         G4double        angleInIn,   // the normal angle of the input face
				         G4Material*     vacuumMaterialIn = NULL,
				         G4double        beamPipeThicknessIn = 0,
				         G4Material*     beamPipeMaterialIn = NULL,
				         G4double        aper1 = 0,
				         G4double        aper2 = 0,
				         G4double        aper3 = 0,
				         G4double        aper4 = 0
				         );

  BDSBeamPipe* CreateBeamPipeAngledOut(  BDSBeamPipeType beamPipeTypeIn,
				         G4String        nameIn,
				         G4double        lengthIn,
				         G4double        angleOutIn, // the normal angle of the output face
				         G4Material*     vacuumMaterialIn = NULL,
				         G4double        beamPipeThicknessIn = 0,
				         G4Material*     beamPipeMaterialIn = NULL,
				         G4double        aper1 = 0,
				         G4double        aper2 = 0,
				         G4double        aper3 = 0,
				         G4double        aper4 = 0
				         );

  BDSBeamPipe* CreateBeamPipeAngledInOut(BDSBeamPipeType beamPipeTypeIn,
					 G4String        nameIn,
					 G4double        lengthIn,
					 G4double        angleInIn,  // the normal angle of the input face
					 G4double        angleOutIn, // the normal angle of the output face
					 G4Material*     vacuumMaterialIn = NULL,
					 G4double        beamPipeThicknessIn = 0,
					 G4Material*     beamPipeMaterialIn = NULL,
					 G4double        aper1 = 0,
					 G4double        aper2 = 0,
					 G4double        aper3 = 0,
					 G4double        aper4 = 0
					 );

private:
  BDSBeamPipeFactory();
  static BDSBeamPipeFactory* _instance;
  BDSBeamPipeFactoryBase* GetAppropriateFactory(BDSBeamPipeType beamPipeTypeIn);

};



#endif
