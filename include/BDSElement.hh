/* BDSIM code.    Version 1.0
   Author: John C. Carter, Royal Holloway, Univ. of London.
   Last modified 02.12.2004
   Copyright (c) 2004 by J.C.Carter.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSElement_h
#define BDSElement_h 

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSMaterials.hh"
#include "G4LogicalVolume.hh"

#include "G4FieldManager.hh"
#include "G4ChordFinder.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4Mag_EqRhs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4UniformMagField.hh"
#include "BDSField.hh"
#include "BDSMagField.hh"
#include "G4CachedMagneticField.hh"
#include "BDSXYMagField.hh"

#include "G4EqMagElectricField.hh"
#include "BDSRK4Stepper.hh"

class BDSElement :public BDSAcceleratorComponent
{
public:
  BDSElement(G4String aName, G4String geometry, G4String bmap, G4double aLength, 
             G4double bpRad, G4double outR, G4String aTunnelMaterial="", G4double tunnelRadius=0., G4double tunnelOffsetX=BDSGlobalConstants::Instance()->GetTunnelOffsetX(), G4String aTunnelCavityMaterial="Air", int aPrecisionRegion=0, G4double fieldZOffset=0);
  ~BDSElement();

  void BuildElementMarkerLogicalVolume();
  void BuildGeometry();
  void PlaceComponents(G4String geometry, G4String bmap);
  void BuildMagField(G4bool forceToAllDaughters=false);

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
   
protected:
  
  G4VisAttributes* SetVisAttributes();  

private:
  G4String itsFieldVolName;
  G4bool itsFieldIsUniform;
  G4ChordFinder* fChordFinder;

  G4MagIntegratorStepper* itsFStepper;
  G4EqMagElectricField* itsFEquation;
  G4Mag_UsualEqRhs* itsEqRhs;

  BDSField *itsField;
  BDSMagField *itsMagField;
  G4CachedMagneticField *itsCachedMagField;
  G4UniformMagField *itsUniformMagField;
  G4double itsOuterR;
  // Volume to align incoming beamline on inside the marker volume
  // (set during Geometery construction)
  G4VPhysicalVolume* align_in_volume;
  // Volume to align outgoing beamline on inside the marker volume
  // (set during Geometery construction)
  G4VPhysicalVolume* align_out_volume;

};


#endif
