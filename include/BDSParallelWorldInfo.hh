#ifndef BDSPARALLELWORLDINFO_H
#define BDSPARALLELWORLDINFO_H

#include "globals.hh"

/**
 * @brief A simple class to hold what parallel geometry is required for each sequence.
 *
 * @author Laurie Nevay
 */

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
  
  G4String sequenceName;      ///< Name of sequence worlds will be associated with.
  G4bool   curvilinearWorld;  ///< Whether a curvilinear world is required.
  G4bool   samplerWorld;      ///< Whether a sampler world is required.
};

#endif
