#ifndef BDSCAVITYELEMENT_H
#define BDSCAVITYELEMENT_H

#include "BDSAcceleratorComponent.hh"

#include "globals.hh" // geant4 globals / types

class BDSCavityInfo;
class BDSFieldInfo;
class G4LogicalVolume;
class G4Material;
class G4VSolid;

/**
 * @brief Base class for cavities.
 * 
 * @author Stuart Walker
 */

class BDSCavityElement: public BDSAcceleratorComponent
{
public:
  BDSCavityElement(G4String             name,
		   G4double             length,
		   G4Material*          vacuumMaterialIn,
		   const BDSFieldInfo*  vacuumField,
		   const BDSCavityInfo* cavityInfoIn);

  virtual ~BDSCavityElement();

protected:
  /// Creates container solid and logical volume
  virtual void BuildContainerLogicalVolume() override;

  /// The full construction sequence that calls all methods in order
  virtual void Build() override;
  
  /// Creates field objects - doesn't nothing by default and derived classes can override.
  virtual void BuildField();

  /// Field information - also includes cavity info as cavity info contains both
  /// field information and geometrical information.
  const BDSFieldInfo* vacuumField = nullptr;

  /// Convenience shortcut to cavity information inside field information object.
  const BDSCavityInfo* cavityInfo = nullptr;

  /*
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
  */
private:
  /// Private constructor to force the use of the provided one.
  BDSCavityElement() = delete;

  /// @{ Assignment and copy constructor not implemented nor used
  BDSCavityElement& operator=(const BDSCavityElement&) = delete;
  BDSCavityElement(BDSCavityElement&) = delete;
  /// @}

  /// Cache of material to be used for vacuum - does not own it.
  G4Material* vacuumMaterial;

  G4LogicalVolume* vacuumLV; ///< Cache of which volume is the vacuum one.
};

#endif
