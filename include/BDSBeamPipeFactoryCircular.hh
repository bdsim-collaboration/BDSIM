#ifndef BDSBEAMPIPEFACTORYCIRCULAR_H
#define BDSBEAMPIPEFACTORYCIRCULAR_H

#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipe.hh"

/**
 * @brief factory for circular beam pipes
 * 
 * singleton pattern
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSBeamPipeFactoryCircular: public BDSBeamPipeFactoryBase
{
public:
  static BDSBeamPipeFactoryCircular* Instance(); /// singleton accessor
  
  virtual ~BDSBeamPipeFactoryCircular();

  virtual BDSBeamPipe* CreateBeamPipe(G4String    nameIn,                     // name
				      G4double    lengthIn,                   // length [mm]
				      G4double    aper1 = 0,                  // aperture parameter 1
				      G4double    aper2 = 0,                  // aperture parameter 2
				      G4double    aper3 = 0,                  // aperture parameter 3
				      G4double    aper4 = 0,                  // aperture parameter 4
				      G4Material* vacuumMaterialIn = NULL,    // vacuum material
				      G4double    beamPipeThicknessIn = 0,    // beampipe thickness [mm]
				      G4Material* beamPipeMaterialIn = NULL   // beampipe material
				      );
  
  virtual BDSBeamPipe* CreateBeamPipeAngledIn(   G4String    nameIn,
					         G4double    lengthIn,
					         G4double    angleInIn,   // the normal angle of the input face
					         G4double    aper1 = 0,
					         G4double    aper2 = 0,
					         G4double    aper3 = 0,
					         G4double    aper4 = 0,
						 G4Material* vacuumMaterialIn = NULL,
					         G4double    beamPipeThicknessIn = 0,
					         G4Material* beamPipeMaterialIn = NULL
					         );
  
  virtual BDSBeamPipe* CreateBeamPipeAngledOut(  G4String    nameIn,
					         G4double    lengthIn,
					         G4double    angleOutIn, // the normal angle of the output face
					         G4double    aper1 = 0,
					         G4double    aper2 = 0,
					         G4double    aper3 = 0,
					         G4double    aper4 = 0,
						 G4Material* vacuumMaterialIn = NULL,
					         G4double    beamPipeThicknessIn = 0,
					         G4Material* beamPipeMaterialIn = NULL
					         );
  
  virtual BDSBeamPipe* CreateBeamPipeAngledInOut(G4String    nameIn,
						 G4double    lengthIn,
						 G4double    angleInIn,  // the normal angle of the input face
						 G4double    angleOutIn, // the normal angle of the output face
						 G4double    aper1 = 0,
						 G4double    aper2 = 0,
						 G4double    aper3 = 0,
						 G4double    aper4 = 0,
						 G4Material* vacuumMaterialIn = NULL,
						 G4double    beamPipeThicknessIn = 0,
						 G4Material* beamPipeMaterialIn = NULL
						 );

private:
  BDSBeamPipeFactoryCircular(); /// private default constructor - singelton pattern
  static BDSBeamPipeFactoryCircular* _instance;

  void TestInputParameters(G4Material*& vacuumMaterialIn,
			   G4double&    beamPipeThicknessIn,
			   G4Material*& beamPipeMaterialIn,
			   G4double&    aper1In);

  G4double lengthSafety;

  //abstract common build features to one function
  //use member variables unique to this factory to pass them around

  BDSBeamPipe* CommonFinalConstruction(G4String    nameIn,
				       G4Material* vacuumMaterialIn,
				       G4Material* beamPipeMaterialIn,
				       G4double    lengthIn,
				       G4double    containerRadiusIn);
  void CreateGeneralAngledSolids(G4String      nameIn,
				 G4double      lengthIn,
				 G4double      aper1In,
				 G4double      beamPipeThicknessIn,
				 G4ThreeVector inputfaceIn,
				 G4ThreeVector outputfaceIn);
  G4VSolid*        vacuumSolid;
  G4VSolid*        beamPipeSolid;
  G4VSolid*        containerSolid;
  G4VSolid*        containerSubtractionSolid;
  G4LogicalVolume* vacuumLV;
  G4LogicalVolume* beamPipeLV;
  G4LogicalVolume* containerLV;

};
  
#endif
