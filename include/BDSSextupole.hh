#ifndef BDSSEXTUPOLE_H
#define BDSSEXTUPOLE_H

#include "globals.hh"

#include "BDSMagnet.hh"

class  BDSBeamPipeInfo;
struct BDSMagnetOuterInfo;

class BDSSextupole: public BDSMagnet
{
public:
  BDSSextupole(G4String            name,
	       G4double            length,
	       G4double            bDblPrime,
	       BDSBeamPipeInfo*    beamPipeInfo,
	       BDSMagnetOuterInfo* magnetOuterInfo);
  ~BDSSextupole(){;};

private:
  G4double itsBDblPrime;

  virtual void Build();
  virtual void BuildBPFieldAndStepper();
  
};

#endif
