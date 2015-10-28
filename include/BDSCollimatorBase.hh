#ifndef BDSCollimator_h
#define BDSCollimator_h 

#include "BDSAcceleratorComponent.hh"
#include "BDSMaterials.hh"

#include "globals.hh" // geant4 types / globals
#include "G4LogicalVolume.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4VSolid.hh"
#include "G4SubtractionSolid.hh"

class G4VSolid;

class BDSCollimatorBase :public BDSAcceleratorComponent
{
public:
  BDSCollimatorBase(G4String name,
		    G4double length,
		    G4double outerDiameter,
		    G4String type,
		    G4double xApertureIn = 0,
		    G4double yApertureIn = 0,
            G4double xOutApertureIn = 0,
            G4double yOutApertureIn = 0,
		    G4String collimatorMaterial = "copper",
		    G4String vacuumMaterial     = "vacuum");
  ~BDSCollimatorBase();

protected:
  virtual void Build();
  
  virtual void BuildContainerLogicalVolume();

  /// Pure virtual function to be provided by derived classes.
  /// Must produce vacuumSolid and innerSolid - the inner is used
  /// to subtract from the mass and the vacuum is placed inside it all
  virtual void BuildInnerCollimator() = 0;

  // Geometrical objects:
  G4VSolid* collimatorSolid;

  G4VSolid* innerSolid;
  G4VSolid* vacuumSolid;

  G4double outerDiameter;
  G4double xAperture;
  G4double yAperture;
  G4double xOutAperture;
  G4double yOutAperture;
  G4String collimatorMaterial;
  G4String vacuumMaterial;

};

#endif
