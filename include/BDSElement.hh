#ifndef BDSELEMENT_H
#define BDSELEMENT_H

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSMaterials.hh"
#include "BDSMagFieldMesh.hh"

#include "G4CachedMagneticField.hh"
#include "G4ChordFinder.hh"
#include "G4EqMagElectricField.hh"
#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"
#include "G4UniformMagField.hh"

/**
 * @brief A class that allows any arbritary geometry and magnetic field map to be used
 * as an accelerator component in the beamline. Geometry and magnetic fields are imported
 * from an external file (each) and can be specified in various formats.
 *
 */

class BDSGeometry;

class BDSElement: public BDSAcceleratorComponent
{
public:
  BDSElement(G4String      name,
	     G4double      length,
	     G4double      outerDiameterIn,
	     G4String      geometry,
	     G4String      bmap,
	     G4ThreeVector bMapOffsetIn);
  ~BDSElement();

  /// Creates a field mesh in global coordinates in case it is given by map
  //void PrepareField(G4VPhysicalVolume *referenceVolume);

  /// Rotates and positions the marker volume before it is placed in
  /// BDSDetectorConstruction. It aligns the marker volume so that the
  /// the beamline goes through the specified daugther volume (e.g. for mokka)
  void AlignComponent(G4ThreeVector& TargetPos, 
		      G4RotationMatrix *TargetRot,
		      G4RotationMatrix& globalRotation,
		      G4ThreeVector& rtot,
		      G4ThreeVector& rlast,
		      G4ThreeVector& localX,
		      G4ThreeVector& localY,
		      G4ThreeVector& localZ); 
   
private:
  /// Overridden method of BDSAcceleratorComponent that defines the build procedure
  /// for this object. Calls BDSAcceleratorComponent::Build() first that builds the
  /// container volume (using BuildContainerLogicalVolume() provided here). Then builds
  /// the geometry and magnetic field maps from the supplied file.
  virtual void Build();

  /// Required implementation from BDSAcceleratorComponent that builds the container volume.
  /// Here, this method uses the outerDiameter parameter from the constructor.
  virtual void BuildContainerLogicalVolume();

  /// Load the geometry and place the components inside the container logical volume.
  void PlaceComponents();

  /// Build the magnetic field
  //void BuildMagField(G4bool forceToAllDaughters = false);

  G4double outerDiameter;
  G4String geometryFileName;
  G4String bMapFileName;

  /// Displacement of b field coordinates from geometry coordinates
  G4ThreeVector bMapOffset;

  G4String itsFieldVolName;
  //G4CachedMagneticField *itsCachedMagField;

  // Volume to align incoming beamline on inside the marker volume
  // (set during Geometery construction)
  G4VPhysicalVolume* align_in_volume;
  // Volume to align outgoing beamline on inside the marker volume
  // (set during Geometery construction)
  G4VPhysicalVolume* align_out_volume;


  BDSGeometry* geometry;
};


#endif
