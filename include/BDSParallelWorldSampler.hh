#ifndef BDSPARALLELWORLDSAMPER_H
#define BDSPARALLELWORLDSAMPER_H

#include "G4VUserParallelWorld.hh"

class BDSParallelWorldSampler: public G4VUserParallelWorld
{
  BDSParallelWorldSampler();
  virtual ~BDSParallelWorldSampler(){;}

  /// Construct the required parallel world geometry. This must
  /// overload the pure virutal method in G4VUserParallelWorld.
  virtual void Construct();


};

#endif
