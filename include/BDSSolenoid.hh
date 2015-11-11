#ifndef BDSSOLENOID_H
#define BDSSOLENOID_H

#include "globals.hh"

#include "BDSMagnet.hh"

class  BDSBeamPipeInfo;
struct BDSMagnetOuterInfo;

class BDSSolenoid: public BDSMagnet
{
public:
  BDSSolenoid(G4String            name,
	      G4double            length,
	      G4double            bField,
	      BDSBeamPipeInfo*    beamPipeInfo,
	      BDSMagnetOuterInfo* magnetOuterInfo);
  ~BDSSolenoid(){;};

private:
  G4double itsBField;

  virtual void Build();
  virtual void BuildBPFieldAndStepper();
};

#endif

