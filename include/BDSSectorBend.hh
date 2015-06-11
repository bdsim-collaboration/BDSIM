#ifndef BDSSECTORBEND_H
#define BDSSECTORBEND_H 

#include "globals.hh"

#include "BDSMultipole.hh"
#include "BDSTiltOffset.hh"

#include <list>

struct BDSBeamPipeInfo;
struct BDSMagnetOuterInfo;

class BDSSectorBend :public BDSMultipole
{
public:
  BDSSectorBend(G4String           nameIn,
		G4double           lengthIn,
		G4double           angleIn,
		G4double           bFieldIn,
		G4double           bGradIn,
		BDSBeamPipeInfo*   beamPipeInfo,
		BDSMagnetOuterInfo magnetOuterInfo,
		BDSTiltOffset      tiltOffset = BDSTiltOffset());
  ~BDSSectorBend(){;};

private:
  G4double itsBField;
  G4double itsBGrad;
  
  /// orientation of shifts - depends on angle - calculations use absolute value of angle for safety
  G4int orientation;

  virtual void Build();
  virtual void BuildBPFieldAndStepper();
  virtual void BuildBeampipe();
};

#endif
