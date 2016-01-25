#ifndef BDSPARALLELWORLDSAMPER_H
#define BDSPARALLELWORLDSAMPER_H

#include "G4VPhysicalVolume.hh"
#include "G4VUserParallelWorld.hh"

#include <vector>

class BDSParallelWorldSampler: public G4VUserParallelWorld
{
  BDSParallelWorldSampler();
  virtual ~BDSParallelWorldSampler();

  /// Construct the required parallel world geometry. This must
  /// overload the pure virutal method in G4VUserParallelWorld.
  virtual void Construct();

private:

  /// Cache of the placements to clean up at the end.
  std::vector<G4VPhysicalVolume*> placements;
};

#endif
