#ifndef BDSRBend_h
#define BDSRBend_h 

#include "globals.hh"

#include "BDSBeamPipeInfo.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMultipole.hh"
#include "BDSTunnelInfo.hh"

class BDSRBend :public BDSMultipole
{
public:
  BDSRBend(G4String           name,
	   G4double           length,
	   G4double           bField,
	   G4double           bGrad,
	   G4double           angle,
	   BDSBeamPipeInfo*   beamPipeInfo,
	   BDSMagnetOuterInfo magnetOuterInfo);
  ~BDSRBend(){;};

private:
  G4double itsBField;
  G4double itsBGrad;
  G4double itsMagFieldLength;

  /// chord length of straight section (along main chord) [m]
  G4double itsStraightSectionChord;

  /// length of little straight sections on either side of dipole [m]
  G4double itsStraightSectionLength;

  /// x shift for magnet and beampipe from chord
  G4double magnetXShift;

  /// orientation of shifts - depends on angle - calculations use absolute value of angle for safety
  G4int orientation;

  /// radius of magnet body
  G4double outerRadius;
  
  virtual void Build();
  virtual void BuildBPFieldAndStepper();
  virtual void BuildBeampipe();
  virtual void BuildOuterVolume(); // override this method to change length used

  /// temporary function while old constructor still exists - used to avoid duplicating
  /// code in the mean time
  void CommonConstructor(G4double aLength);
};

#endif
