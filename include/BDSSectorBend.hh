
#ifndef BDSSECTORBEND_H
#define BDSSECTORBEND_H 

#include "globals.hh"

#include "BDSMagnet.hh"

class  BDSBeamPipeInfo;
struct BDSMagnetOuterInfo;

class BDSSectorBend: public BDSMagnet
{
public:
  BDSSectorBend(G4String            nameIn,
		G4double            lengthIn,
		G4double            angleIn,
		G4double            bFieldIn,
		G4double            bGradIn,
		G4double            e1in,
		G4double            e2in,
		BDSBeamPipeInfo*    beamPipeInfo,
		BDSMagnetOuterInfo* magnetOuterInfo);
  ~BDSSectorBend(){;};

private:
  G4double itsBField;
  G4double itsBGrad;
  G4double e1;
  G4double e2;
  
  virtual void Build();
  virtual void BuildBPFieldAndStepper();
  virtual void BuildBeampipe();
  virtual void BuildOuter();

};

#endif
