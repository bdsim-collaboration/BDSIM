#ifndef BDSPARALLELWORLDINFO_H
#define BDSPARALLELWORLDINFO_H

#include "globals.hh"

class BDSParallelWorldInfo
{
public:
  BDSParallelWorldInfo();
  BDSParallelWorldInfo(G4String sequenceNameIn,
		       G4bool   curvilinearWorldIn,
		       G4bool   samplerWorldIn):
    sequenceName(sequenceNameIn),
    curvilinearWorld(curvilinearWorldIn),
    samplerWorld(samplerWorldIn)
  {;}
  ~BDSParallelWorldInfo(){;}
  
  G4String sequenceName;
  G4bool   curvilinearWorld;
  G4bool   samplerWorld;
};

#endif
