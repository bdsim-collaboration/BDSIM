#ifndef BDSQUADRUPOLE_H
#define BDSQUADRUPOLE_H

#include "globals.hh"
#include "BDSMagnet.hh"

class  BDSBeamPipeInfo;
struct BDSMagnetOuterInfo;

class BDSQuadrupole: public BDSMagnet
{
public:
  BDSQuadrupole(G4String            name,
		G4double            length,
		G4double            bGrad,
		BDSBeamPipeInfo*    beamPipeInfo,
		BDSMagnetOuterInfo* magnetOuterInfo);
		
  ~BDSQuadrupole(){;}

private:
  G4double itsBGrad;
  
  virtual void Build();
  virtual void BuildBPFieldAndStepper();
};

#endif
