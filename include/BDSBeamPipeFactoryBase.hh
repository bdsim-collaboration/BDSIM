#ifndef BDSBEAMPIPEFACTORYBASE_H
#define BDSBEAMPIPEFACTORYBASE_H

#include "BDSBeamPipe.hh"

#include "globals.hh"         // geant4 globals / types
#include "G4Material.hh"      // materials

class BDSExtent;

class G4LogicalVolume;
class G4PVPlacement;
class G4UserLimits;
class G4VSolid;

/**
 * @brief Abstract base class for beampipe factory classes.
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
 * ie beampipe material will be steel - the default from options.
 *
 * NOTE each derived class is expected to be a singleton but this can't be specified here
 * as it'd refer to the abstract base class - must be implemented in each derived class
 * utilised in BDSBeamPipeFactory.cc
 *
 * 'In' in argument variables is used to distinguish between that and possible
 * internal member variables with the same name - avoiding 'itsVariable'
 * 
 * @author Laurie Nevay
 */

class BDSBeamPipeFactoryBase
{
public:
  /// create a flat ended beampipe
  virtual BDSBeamPipe* CreateBeamPipe(G4String    nameIn,                      // name
				      G4double    lengthIn,                    // length [mm]
				      G4double    aper1 = 0,                   // aperture parameter 1
				      G4double    aper2 = 0,                   // aperture parameter 2
				      G4double    aper3 = 0,                   // aperture parameter 3
				      G4double    aper4 = 0,                   // aperture parameter 4
				      G4Material* vacuumMaterialIn = nullptr,  // vacuum material
				      G4double    beamPipeThicknessIn = 0,     // beampipe thickness [mm]
				      G4Material* beamPipeMaterialIn = nullptr // beampipe material
				      ) = 0;

  /// Create a beam pipe with angled faces as described by unit normal vectors. The input
  /// face normal vector must have a negative z component and the output face a positive
  /// z component.
  virtual BDSBeamPipe* CreateBeamPipe(G4String      nameIn,
				      G4double      lengthIn,
				      G4ThreeVector inputFaceNormalIn,
				      G4ThreeVector outputFaceNormalIn,
				      G4double      aper1               = 0,
				      G4double      aper2               = 0,
				      G4double      aper3               = 0,
				      G4double      aper4               = 0,
				      G4Material*   vacuumMaterialIn    = nullptr,
				      G4double      beamPipeThicknessIn = 0,
				      G4Material*   beamPipeMaterialIn  = nullptr) = 0;

protected:
  /// base constructor
  BDSBeamPipeFactoryBase();

  /// Initialiser and can be used to reset factory pointers after use
  virtual void CleanUp();
  
  /// finalise beampipe construction
  void CommonConstruction(G4String    nameIn,
			  G4Material* vacuumMaterialIn,
			  G4Material* beamPipeMaterialIn,
			  G4double    lengthIn);

  /// build beampipe and register logical volumes
  BDSBeamPipe* BuildBeamPipeAndRegisterVolumes(BDSExtent extent,
					       G4double  containerRadius);
  
  // methods called by CommonConstruction, can be implmented by derived classes
  
  /// build logical volumes
  virtual void BuildLogicalVolumes(G4String    nameIn,
				   G4Material* vacuumMaterialIn,
				   G4Material* beamPipeMaterialIn);
  /// Set visual attributes.
  virtual void          SetVisAttributes();

  /// Set user limits.
  virtual G4UserLimits* SetUserLimits(G4double lengthIn);

  /// Place volumes.
  virtual void          PlaceComponents(G4String nameIn);

  /// A local copy of global length safety variable.
  G4double         lengthSafety;

  /// 1um safety that can be used for larger transverse safety.
  G4double         lengthSafetyLarge;
  
  G4bool           checkOverlaps;
  G4double         maxStepFactor;
  G4double         nSegmentsPerCircle; ///< For visualisation improvement
  G4VSolid*        vacuumSolid;
  G4VSolid*        beamPipeSolid;
  G4VSolid*        containerSolid;
  /// Longer (in length) version of container solid for unambiguous subtraction.
  G4VSolid*        containerSubtractionSolid; 
  G4LogicalVolume* vacuumLV;
  G4LogicalVolume* beamPipeLV;
  G4LogicalVolume* containerLV;
  G4PVPlacement*   vacuumPV;
  G4PVPlacement*   beamPipePV;

  /// @{ For non standard parts for easy registration - ie not the specific ones above.
  std::vector<G4LogicalVolume*>   allLogicalVolumes;
  std::vector<G4VPhysicalVolume*> allPhysicalVolumes;
  std::vector<G4RotationMatrix*>  allRotationMatrices;
  std::vector<G4VSolid*>          allSolids;
  std::vector<G4VisAttributes*>   allVisAttributes;
  std::vector<G4UserLimits*>      allUserLimits;
  /// @}
  
  /// @{ For recording the face normals in the finished pipe component.
  G4ThreeVector inputFaceNormal;
  G4ThreeVector outputFaceNormal;
};

#endif
