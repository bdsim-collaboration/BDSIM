#ifndef BDSBEAMPIPEFACTORYRECTANGULAR_H
#define BDSBEAMPIPEFACTORYRECTANGULAR_H

#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipe.hh"

/**
 * @brief Factory for rectangular beam pipes
 * 
 * singleton pattern
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSBeamPipeFactoryRectangular: public BDSBeamPipeFactoryBase
{
public:
  static BDSBeamPipeFactoryRectangular* Instance(); /// singleton accessor
  
  virtual ~BDSBeamPipeFactoryRectangular();

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
  BDSBeamPipeFactoryRectangular(); /// private default constructor - singelton pattern
  static BDSBeamPipeFactoryRectangular* _instance;

  void TestInputParameters(G4Material*& vacuumMaterialIn,
			   G4double&    beamPipeThicknessIn,
			   G4Material*& beamPipeMaterialIn,
			   G4double&    aper1In,
			   G4double&    aper2In);

  G4double lengthSafety;

  //abstract common build features to one function
  //use member variables unique to this factory to pass them around
  BDSBeamPipe* CommonFinalConstruction(G4String    nameIn,
				       G4Material* vacuumMaterialIn,
				       G4Material* beamPipeMaterialIn,
				       G4double    lengthIn,
				       G4double    aper1In,
				       G4double    aper2In,
				       G4double    beamPipeThicknessIn);
  void CreateGeneralAngledSolids(G4String      nameIn,
				 G4double      lengthIn,
				 G4double      aper1In,
				 G4double      aper2In,
				 G4double      beamPipeThicknessIn,
				 G4ThreeVector inputfaceIn,
				 G4ThreeVector outputfaceIn);
};
  
#endif
