#ifndef BDSBEAMPIPEFACTORYLHCDETAILED_H
#define BDSBEAMPIPEFACTORYLHCDETAILED_H

#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipe.hh"

/**
 * @brief Factory for detailed lhc aperture model beam pipes
 * 
 * singleton pattern
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSBeamPipeFactoryLHCDetailed: public BDSBeamPipeFactoryBase
{
public:
  static BDSBeamPipeFactoryLHCDetailed* Instance(); ///< singleton accessor
  
  virtual ~BDSBeamPipeFactoryLHCDetailed();

  /// Access cooling pipe information from factory for parameter tests
  G4double GetFullWidthOfCoolingPipe();

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
  
  virtual BDSBeamPipe* CreateBeamPipeAngledIn(G4String    nameIn,
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
  
  virtual BDSBeamPipe* CreateBeamPipeAngledOut(G4String    nameIn,
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
  BDSBeamPipeFactoryLHCDetailed(); ///< private default constructor - singleton pattern
  static BDSBeamPipeFactoryLHCDetailed* _instance;

  void TestInputParameters(G4Material*& vacuumMaterialIn,
			   G4double&    beamPipeThicknessIn,
			   G4Material*& beamPipeMaterialIn,
			   G4double&    aper1In,
			   G4double&    aper2In,
			   G4double&    aper3In);

  G4double lengthSafety;
  G4double coldBoreThickness;
  G4double coolingPipeThickness;
  G4double coolingPipeRadius;
  G4double coolingPipeYOffset;
  G4double copperSkinThickness;

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
				 G4double      aper2In,
				 G4double      aper3In,
				 G4double      beamPipeThicknessIn,
				 G4ThreeVector inputfaceIn,
				 G4ThreeVector outputfaceIn);

  virtual void          BuildLogicalVolumes(G4String    nameIn,
					    G4Material* vacuumMaterialIn,
					    G4Material* beamPipeMaterialIn);
  virtual void          SetVisAttributes();
  virtual G4UserLimits* SetUserLimits(G4double lengthIn);
  virtual void          PlaceComponents(G4String nameIn);

  G4VSolid*        copperSkinSolid;
  G4VSolid*        screenSolid;
  G4VSolid*        coolingPipeSolid;
  G4LogicalVolume* copperSkinLV;
  G4LogicalVolume* screenLV;
  G4LogicalVolume* coolingPipeLV;
};
  
#endif
