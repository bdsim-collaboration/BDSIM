#ifndef __BDSSOLENOID_H
#define __BDSSOLENOID_H

#include "globals.hh"

#include "BDSMultipole.hh"
#include "BDSTiltOffset.hh"

#include <list>

struct BDSBeamPipeInfo;
struct BDSMagnetOuterInfo;

class BDSSolenoid : public BDSMultipole
{
public:
  BDSSolenoid(G4String           name,
	      G4double           length,
	      G4double           bField,
	      BDSBeamPipeInfo*   beamPipeInfo,
	      BDSMagnetOuterInfo magnetOuterInfo,
	      BDSTiltOffset      tiltOffset = BDSTiltOffset());
  ~BDSSolenoid(){;};

private:
  G4double itsBField;

  virtual void Build();
  virtual void BuildBPFieldAndStepper();
};

#endif

