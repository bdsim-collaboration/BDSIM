/* BDSIM code.    Version 1.0
   Author: John C. Carter, Royal Holloway, Univ. of London.
   Last modified 02.12.2004
   Copyright (c) 2004 by J.C.Carter.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSELEMENT_H
#define BDSELEMENT_H 

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSMaterials.hh"
#include "BDSMagField.hh"

#include "G4CachedMagneticField.hh"
#include "G4ChordFinder.hh"
#include "G4EqMagElectricField.hh"
#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4Mag_EqRhs.hh"
#include "G4UserLimits.hh"
#include "G4UniformMagField.hh"

/**
 * @brief A class that allows any arbritary geometry and magnetic field map to be used
 * as an accelerator component in the beamline. Geometry and magnetic fields are imported
 * from an external file (each) and can be specified in various formats.
 *
 */

class BDSElement: public BDSAcceleratorComponent
{
public:
  BDSElement(G4String      name,
	     G4double      length,
	     G4double      outerDiameterIn,
	     G4String      geometry,
	     G4String      bmap,
	     G4double      aBmapZOffset);
  ~BDSElement();

  // creates a field mesh in global coordinates in case it is given by map
  void PrepareField(G4VPhysicalVolume *referenceVolume);

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
  void PlaceComponents(G4String geometry, G4String bmap);

  /// Build the magnetic field
  void BuildMagField(G4bool forceToAllDaughters=false);

  G4double outerDiameter;
  
  G4String itsGeometry;
  G4String itsBmap;

  G4String itsFieldVolName;
  G4bool itsFieldIsUniform;
  G4ChordFinder* fChordFinder;

  G4MagIntegratorStepper* itsFStepper;
  G4EqMagElectricField* itsFEquation;
  G4Mag_UsualEqRhs* itsEqRhs;

  BDSMagField *itsMagField;
  G4CachedMagneticField *itsCachedMagField;
  G4UniformMagField *itsUniformMagField;
  G4double itsBmapZOffset;
  // Volume to align incoming beamline on inside the marker volume
  // (set during Geometery construction)
  G4VPhysicalVolume* align_in_volume;
  // Volume to align outgoing beamline on inside the marker volume
  // (set during Geometery construction)
  G4VPhysicalVolume* align_out_volume;
};


#endif
