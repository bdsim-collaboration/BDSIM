#ifndef BDSPARALLELWORLDSAMPER_H
#define BDSPARALLELWORLDSAMPER_H

#include "G4VPhysicalVolume.hh"
#include "G4VUserParallelWorld.hh"

#include <vector>

class G4VisAttributes;
class BDSSampler;

/**
 * @brief A parallel world for sampler planes.
 *
 * @author Laurie Nevay
 */

class BDSParallelWorldSampler: public G4VUserParallelWorld
{
public:
  BDSParallelWorldSampler();
  virtual ~BDSParallelWorldSampler();

  /// Construct the required parallel world geometry. This must
  /// overload the pure virutal method in G4VUserParallelWorld.
  virtual void Construct();

private: 
  /// Cache of the placements to clean up at the end.
  std::vector<G4VPhysicalVolume*> placements;

  /// Cache of samplers to delete at the end
  std::vector<BDSSampler*> samplers;

  /// Visualisation attributes for the sampler world.
  G4VisAttributes* samplerWorldVis;
};

#endif
