#ifndef BDSBEAMPIPEFACTORY_H
#define BDSBEAMPIPEFACTORY_H

#include "BDSBeamPipeType.hh"

#include "globals.hh"           // geant4 globals / types

class G4Material;

class BDSBeamPipe;
class BDSBeamPipeInfo;
class BDSBeamPipeFactoryBase;

/**
 * @brief The main interface for using the beam pipe factories. 
 * 
 * Each function in this class mirrors
 * one in BDSBeamPipeFactoryBase but with the addition of a BDSBeamPipeType enum as an argument. This
 * factory dispatches the call to the correct factory and should be the only one used by the caller.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSBeamPipeFactory
{
public:
  static BDSBeamPipeFactory* Instance(); /// singleton pattern
  
  ~BDSBeamPipeFactory();

  BDSBeamPipe* CreateBeamPipe(G4String         name,
			      G4double         length,
			      BDSBeamPipeInfo* bpi);
  
  BDSBeamPipe* CreateBeamPipe(BDSBeamPipeType beamPipeTypeIn,            // aperture type
			      G4String        nameIn,                    // name
			      G4double        lengthIn,                  // length [mm]
			      G4double        aper1 = 0,                 // aperture parameter 1
			      G4double        aper2 = 0,                 // aperture parameter 2
			      G4double        aper3 = 0,                 // aperture parameter 3
			      G4double        aper4 = 0,                 // aperture parameter 4
			      G4Material*     vacuumMaterialIn = nullptr,   // vacuum material
			      G4double        beamPipeThicknessIn = 0,   // beampipe thickness [mm]
			      G4Material*     beamPipeMaterialIn = nullptr  // beampipe material
			      );

  BDSBeamPipe* CreateBeamPipeAngledIn(   BDSBeamPipeType beamPipeTypeIn,
				         G4String        nameIn,
				         G4double        lengthIn,
				         G4double        angleInIn,   // the normal angle of the input face
				         G4double        aper1 = 0,
				         G4double        aper2 = 0,
				         G4double        aper3 = 0,
				         G4double        aper4 = 0,
					 G4Material*     vacuumMaterialIn = nullptr,
				         G4double        beamPipeThicknessIn = 0,
				         G4Material*     beamPipeMaterialIn = nullptr
				         );

  BDSBeamPipe* CreateBeamPipeAngledOut(  BDSBeamPipeType beamPipeTypeIn,
				         G4String        nameIn,
				         G4double        lengthIn,
				         G4double        angleOutIn, // the normal angle of the output face
				         G4double        aper1 = 0,
				         G4double        aper2 = 0,
				         G4double        aper3 = 0,
				         G4double        aper4 = 0,
					 G4Material*     vacuumMaterialIn = nullptr,
				         G4double        beamPipeThicknessIn = 0,
				         G4Material*     beamPipeMaterialIn = nullptr
				         );

  BDSBeamPipe* CreateBeamPipeAngledInOut(BDSBeamPipeType beamPipeTypeIn,
					 G4String        nameIn,
					 G4double        lengthIn,
					 G4double        angleInIn,  // the normal angle of the input face
					 G4double        angleOutIn, // the normal angle of the output face
					 G4double        aper1 = 0,
					 G4double        aper2 = 0,
					 G4double        aper3 = 0,
					 G4double        aper4 = 0,
					 G4Material*     vacuumMaterialIn = nullptr,
					 G4double        beamPipeThicknessIn = 0,
					 G4Material*     beamPipeMaterialIn = nullptr
					 );

private:
  BDSBeamPipeFactory();
  static BDSBeamPipeFactory* _instance;
  BDSBeamPipeFactoryBase* GetAppropriateFactory(BDSBeamPipeType beamPipeTypeIn);

};



#endif
