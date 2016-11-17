#ifndef BDSMUSPOILER_H
#define BDSMUSPOILER_H

#include "globals.hh"
#include "BDSMagnet.hh"

class  BDSBeamPipeInfo;
struct BDSMagnetOuterInfo;

class BDSMuSpoiler: public BDSMagnet
{
public:
  BDSMuSpoiler(G4String            name,
	       G4double            length,
	       G4double            bField,
	       BDSBeamPipeInfo*    beamPipeInfo,
	       BDSMagnetOuterInfo* magnetOuterInfo);

  ~BDSMuSpoiler(){;}
};

#endif
