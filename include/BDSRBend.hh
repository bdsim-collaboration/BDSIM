#ifndef BDSRBend_h
#define BDSRBend_h 

#include "globals.hh"

#include "BDSMultipole.hh"

class BDSRBend :public BDSMultipole
{
public:
  BDSRBend(G4String aName, G4double aLength,
	   G4double bpRad, G4double FeRad,
	   G4double bField, G4double angle, G4double outR,
           std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
           G4double tilt = 0, G4double bGrad=0, G4String aTunnelMaterial="", G4String aMaterial = "");
  BDSRBend(G4String     name,
	   G4double     length,
	   G4double     bField,
	   G4double     bGrad,
	   G4double     angle,
	   beamPipeInfo beamPipeInfo,
	   G4double     boxSize,
	   G4String     outerMaterial="",
	   G4String     tunnelMaterial="",
	   G4double     tunnelRadius=0,
	   G4double     tunnelOffsetX=0);
  ~BDSRBend(){;};

private:
  G4double itsBField;
  G4double itsBGrad;
  G4double itsMagFieldLength;

  /// chord length in [m]
  G4double itsChordLength;

  /// chord length of straight section (along main chord) [m]
  G4double itsStraightSectionChord;

  /// length of little straight sections on either side of dipole [m]
  G4double itsStraightSectionLength;
  
  /// normal vectors for faces when preparing solids
  G4ThreeVector inputface;
  G4ThreeVector outputface;

  /// x shift for magnet and beampipe from chord
  G4double magnetXShift;

  /// orientation of shifts - depends on angle - calculations use absolute value of angle for safety
  G4int orientation;

  /// radius of magnet body
  G4double outerRadius;
  
  virtual void Build();
  virtual void BuildBPFieldAndStepper();
  virtual void BuildMarkerLogicalVolume();
  virtual void BuildBeampipe(G4String materialName="");
  virtual void BuildOuterLogicalVolume(G4bool OuterMaterialIsVacuum=false);

  virtual void SetVisAttributes();

  /// temporary function while old constructor still exists - used to avoid duplicating
  /// code in the mean time
  void CommonConstructor(G4double aLength);
};

#endif
