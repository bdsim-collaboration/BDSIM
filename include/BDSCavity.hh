#ifndef BDSCAVITY_H
#define BDSCAVITY_H

#include "BDSAcceleratorComponent.hh"

#include "globals.hh" // geant4 globals / types
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "G4VSolid.hh"

class BDSCavityInfo;
class BDSFieldInfo;
class G4Material;
class G4VSolid;

/**
 * @brief Base class for cavities.
 * 
 * @author Stuart Walker
 */

class BDSCavity: public BDSAcceleratorComponent
{
public:
  BDSCavity(G4String            name,
	    G4double            length,
	    const BDSFieldInfo* vacuumField);

  virtual ~BDSCavity();

protected:
  /// Creates container solid and logical volume
  virtual void BuildContainerLogicalVolume() override;

  /// The full construction sequence that calls all methods in order
  virtual void Build() override;
  
  /// Creates field objects - doesn't nothing by default and derived classes can override.
  virtual void BuildField();

  /// Initialises physical volumes.
  void PlaceComponents();

  /// Builds the geometry and the vacuum of this shape.
  void BuildEllipticalCavityGeometry(); 

  /// Builds the pillbox geometry and the vacuum to go indoors.
  void BuildPillBoxCavityGeometry(); 
  
  //Solids  
  G4VSolid* cavitySolid = nullptr; ///<Set by e.g BuildEllipticalCavityGeometry
  G4VSolid* innerSolid  = nullptr; ///<Used only PillBox atm Should expand to elliptical as well
  G4VSolid* vacuumSolid = nullptr; ///<Set by e.g BuildEllipticalCavityGeometry

  G4LogicalVolume* cavityLV = nullptr; ///<Set at same time as cavitySolid
  G4LogicalVolume* vacuumLV = nullptr; ///<Set at same time as vacuumSolid
 
  G4double cavityRadius; ///< Largest value of r from z.
  G4double irisRadius;   ///< Radius of the iris (aperture).
  G4double thickness;    ///< Thickness. Constant thickness. Any deviation is an artifact.

  /// Field information - also includes cavity info as cavity info contains both
  /// field information and geometrical information.
  const BDSFieldInfo* vacuumField = nullptr;

  /// Convenience shortcut to cavity information inside field information object.
  const BDSCavityInfo* cavityInfo = nullptr;

private:
  /// Private constructor to force the use of the provided one.
  BDSCavity() = delete;

  /// @{ Assignment and copy constructor not implemented nor used
  BDSCavity& operator=(const BDSCavity&) = delete;
  BDSCavity(BDSCavity&) = delete;
  /// @}
};

#endif
