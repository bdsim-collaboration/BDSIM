#ifndef BDSBEAMPIPEFACTORY_H
#define BDSBEAMPIPEFACTROY_H

#include "BDSBeamPipe.hh"

#include "globals.hh"           // geant4 globals / types
#include "G4Material.hh"

class BDSBeamPipeFactory
{
public:
  BDSBeamPipeFactory();
  ~BDSBeamPipeFactory();
  
  BDSBeamPipe* createBeamPipe(G4String    apertureTypeIn,            // aperture type
			      G4String    nameIn,                    // name
			      G4double    lengthIn,                  // length [mm]
			      G4Material* vacuumMaterialIn = NULL,   // vacuum material
			      G4double    beamPipeThicknessIn = 0,   // beampipe thickness [mm]
			      G4Material* beamPipeMaterialIn = NULL, // beampipe material
			      G4double    aper1 = 0,                 // aperture parameter 1
			      G4double    aper2 = 0,                 // aperture parameter 2
			      G4double    aper3 = 0,                 // aperture parameter 3
			      G4double    aper4 = 0                  // aperture parameter 4
			      );

  BDSBeamPipe* createBeamPipeAngledIn(G4String    apertureTypeIn,
				      G4String    nameIn,
				      G4double    lengthIn,
				      G4double    angleInIn,   // the normal angle of the input face
				      G4Material* vacuumMaterialIn = NULL,
				      G4double    beamPipeThicknessIn = 0,
				      G4Material* beamPipeMaterialIn = NULL,
				      G4double    aper1 = 0,
				      G4double    aper2 = 0,
				      G4double    aper3 = 0,
				      G4double    aper4 = 0
				      );

  BDSBeamPipe* createBeamPipeAngledOut(G4String    apertureTypeIn,
				       G4String    nameIn,
				       G4double    lengthIn,
				       G4double    angleOutIn, // the normal angle of the output face
				       G4Material* vacuumMaterialIn = NULL,
				       G4double    beamPipeThicknessIn = 0,
				       G4Material* beamPipeMaterialIn = NULL,
				       G4double    aper1 = 0,
				       G4double    aper2 = 0,
				       G4double    aper3 = 0,
				       G4double    aper4 = 0
				       );

  BDSBeamPipe* createBeamPipeAngledInOut(G4String    apertureTypeIn,
					 G4String    nameIn,
					 G4double    lengthIn,
					 G4double    angleInIn,  // the normal angle of the input face
					 G4double    angleOutIn, // the normal angle of the output face
					 G4Material* vacuumMaterialIn = NULL,
					 G4double    beamPipeThicknessIn = 0,
					 G4Material* beamPipeMaterialIn = NULL,
					 G4double    aper1 = 0,
					 G4double    aper2 = 0,
					 G4double    aper3 = 0,
					 G4double    aper4 = 0
					 );

};



#endif
