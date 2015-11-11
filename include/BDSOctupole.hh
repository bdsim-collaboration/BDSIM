#ifndef BDSOCTUPOLE_H
#define BDSOCTUPOLE_H

#include "BDSMagnet.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Material.hh"

class  BDSBeamPipeInfo;
struct BDSMagnetOuterInfo;

class BDSOctupole: public BDSMagnet
{
public:
  BDSOctupole(G4String            name,
	      G4double            length,
	      G4double            bTriplePrime,
	      BDSBeamPipeInfo*    beamPipeInfo,
	      BDSMagnetOuterInfo* magnetOuterInfo);
  ~BDSOctupole(){;};

protected:
  virtual void Build();
  virtual void BuildBPFieldAndStepper();

private:
  G4double itsBTriplePrime;
};

#endif
