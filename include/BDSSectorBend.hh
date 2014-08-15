//  
//   BDSIM, (C) 2001-2007
//   
//   version 0.4
//  
//
//
//   Rectangular bending magnet class
//   - itsLength parameter internally stores the geometrical length 
//   - itsAngle parameter internally stores the bending angle
//   - to get the arc length use the GetArcLength() function
//   - the volume is a trapezoid with pole faces perpendicular to the ideal
//     orbit
//     
//   History
//
//

#ifndef BDSSectorBend_h
#define BDSSectorBend_h 

#include "globals.hh"
#include "BDSMaterials.hh"
#include "G4LogicalVolume.hh"
#include "BDSDipoleStepper.hh"

#include "G4FieldManager.hh"
#include "G4ChordFinder.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"               

#include "BDSMultipole.hh"
#include "BDSSbendMagField.hh"
#include "G4Mag_EqRhs.hh"

class BDSSectorBend :public BDSMultipole
{
public:
  BDSSectorBend(G4String aName, G4double aLength,
                G4double bpRad, G4double FeRad,
                G4double bField, G4double angle, G4double outR,
                std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
                G4double tilt = 0, G4double bGrad=0, 
                G4String aTunnelMaterial="",
                G4String aMaterial = "", G4double xAper=0, G4double yAper=0);
  ~BDSSectorBend();

  virtual G4double GetArcLength();

private:
  G4double itsBField;
  G4double itsBGrad;

  void BuildBPFieldAndStepper();
  void BuildSBMarkerLogicalVolume();
  void BuildSBBeampipe();
  void BuildSBOuterLogicalVolume(G4bool OuterMaterialIsVacuum=false);
  void BuildSBDefaultOuterLogicalVolume(G4bool OuterMaterialIsVacuum=false);

  G4VisAttributes* SetVisAttributes();

  // field related objects:
  BDSDipoleStepper* itsStepper;
  BDSSbendMagField* itsMagField;
  G4Mag_EqRhs* itsEqRhs;

  // G4int itsNSegments;
  // G4double itsSegmentLength;
  // G4double itsSegmentAngle;
};

#endif
