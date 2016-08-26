#ifndef BDSCOLLIMATORBASE_H
#define BDSCOLLIMATORBASE_H

#include "BDSAcceleratorComponent.hh"

#include "globals.hh" // geant4 types / globals

class G4VSolid;

/**
 * @brief Base class for collimators with common construction
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSCollimatorBase: public BDSAcceleratorComponent
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
		    G4String vacuumMaterial     = "vacuum",
		    G4String colourIn           = "collimator");
  ~BDSCollimatorBase();

protected:
  virtual void Build();
  
  virtual void BuildContainerLogicalVolume();

  /// Pure virtual function to be provided by derived classes.
  /// Must produce vacuumSolid and innerSolid - the inner is used
  /// to subtract from the mass and the vacuum is placed inside it all
  virtual void BuildInnerCollimator() = 0;

  ///@{ Geometrical objects:
  G4VSolid* collimatorSolid;

  G4VSolid* innerSolid;
  G4VSolid* vacuumSolid;
  ///@}
  /// Outer diameter
  G4double outerDiameter;
  /// Aperture at entrance in x dimension
  G4double xAperture;
  /// Aperture at entrance in y dimension
  G4double yAperture;
  /// Aperture at exit in x dimension
  G4double xOutAperture;
  /// Aperture at exit in y dimension
  G4double yOutAperture;
  /// Material
  G4String collimatorMaterial;
  /// Vacuum material
  G4String vacuumMaterial;
  /// Colour of collimator
  G4String colour;
};

#endif
