#ifndef BDSBEAMPIPEFACTORYBASE_H
#define BDSBEAMPIPEFACTORYBASE_H

#include "BDSBeamPipe.hh"

#include "globals.hh"         // geant4 globals / types
#include "G4Material.hh"      // materials

class G4UserLimits;

/**
 * @brief abstract base class for beampipe factory classes
 * 
 * Abstract base class that defines the interface of each factory
 * to build any type of beampipe required. There are four general
 * possibilities apart from the shape (each derived class will give
 * a different cross-section) and these dictate the angled faces.
 * 
 * Uses Geant4 default units by default, ie mm, rad (not metres)
 * 
 * Note, any type of aperture (in MADX) requires up to 4 parameters
 * 
 * Default values in constructors result in global defaults being used
 * ie beampipe material will be steel - the default from options
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSBeamPipeFactoryBase
{
public:
  // NOTE each derived class is expected to be a singleton but this can't be specified here
  // as it'd refer to the abstract base class - must be implemented in each derived class
  // utilised in BDSBeamPipeFactory.cc

  // 'In' in argument variables is used to distinguish between that and possible
  // internal member variables with the same name - avoiding 'itsVariable'
  
  /// create a flat ended beampipe
  virtual BDSBeamPipe* CreateBeamPipe(G4String    nameIn,                    // name
				      G4double    lengthIn,                  // length [mm]
				      G4double    aper1 = 0,                 // aperture parameter 1
				      G4double    aper2 = 0,                 // aperture parameter 2
				      G4double    aper3 = 0,                 // aperture parameter 3
				      G4double    aper4 = 0,                 // aperture parameter 4
				      G4Material* vacuumMaterialIn = NULL,   // vacuum material
				      G4double    beamPipeThicknessIn = 0,   // beampipe thickness [mm]
				      G4Material* beamPipeMaterialIn = NULL  // beampipe material
				      ) = 0;

  /// create beampipe with an angled face on input side only
  virtual BDSBeamPipe* CreateBeamPipeAngledIn(   G4String    nameIn,
					         G4double    lengthIn,
					         G4double    angleInIn, // the normal angle of the input face
					         G4double    aper1 = 0,
					         G4double    aper2 = 0,
					         G4double    aper3 = 0,
					         G4double    aper4 = 0,
						 G4Material* vacuumMaterialIn = NULL,
					         G4double    beamPipeThicknessIn = 0,
					         G4Material* beamPipeMaterialIn = NULL
					         ) = 0;

  /// create beampipe with an angled face on output side only
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
					         ) = 0;
  
  /// create beampipe with an angled face on both input adn output sides
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
						 ) = 0;

protected:
  /// finalise beampipe construction
  void CommonConstruction(G4String    nameIn,
			  G4Material* vacuumMaterialIn,
			  G4Material* beamPipeMaterialIn,
			  G4double    lengthIn);

  /// build beampipe and register logical volumes
  BDSBeamPipe* BuildBeamPipeAndRegisterVolumes(std::pair<double,double> extX,
					       std::pair<double,double> extY,
					       std::pair<double,double> extZ,
					       G4double containerRadius);

  /// Calculate orientation
  void CalculateOrientations(G4double angleIn, G4double angleOut);

  //void CreateGeneralAngledSolids();
  void TestInputParameters(G4Material*& vacuumMaterialIn,
			   G4double&    beamPipeThicknessIn,
			   G4Material*& beamPipeMaterialIn);
  
  // methods called by CommonConstruction, can be implmented by derived classes
  
  /// build logical volumes
  virtual void BuildLogicalVolumes(G4String    nameIn,
				   G4Material* vacuumMaterialIn,
				   G4Material* beamPipeMaterialIn);
  /// set visual attributes
  virtual void SetVisAttributes();
  /// set beampipe to sensitive detector (if specified)
  virtual void SetSensitivity();
  /// set user limits
  virtual G4UserLimits* SetUserLimits(G4double lengthIn);
  /// place volumes
  virtual void PlaceComponents(G4String nameIn);

protected:
  G4VSolid*        vacuumSolid;
  G4VSolid*        beamPipeSolid;
  G4VSolid*        containerSolid;
  /// longer (in length) version of container solid for unambiguous subtraction
  G4VSolid*        containerSubtractionSolid; 
  G4LogicalVolume* vacuumLV;
  G4LogicalVolume* beamPipeLV;
  G4LogicalVolume* containerLV;

  /// orientation -1,0,1 value - always use |angle| with trigonometric and then
  /// multiply by this factor, 0 by default - determine this in one function
  /// to avoid mistakes
  G4int orientationIn;
  G4int orientationOut;
};

#endif
