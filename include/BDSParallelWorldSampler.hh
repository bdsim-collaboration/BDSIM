#ifndef BDSPARALLELWORLDSAMPER_H
#define BDSPARALLELWORLDSAMPER_H

#include "G4String.hh"
#include "G4VUserParallelWorld.hh"

#include <vector>

class G4VisAttributes;
class G4VPhysicalVolume;
class BDSSampler;

/**
 * @brief A parallel world for sampler planes.
 *
 * @author Laurie Nevay
 */

class BDSParallelWorldSampler: public G4VUserParallelWorld
{
public:
  explicit BDSParallelWorldSampler(G4String name);
  virtual ~BDSParallelWorldSampler();

  /// Construct the required parallel world geometry. This must
  /// overload the pure virtual method in G4VUserParallelWorld.
  virtual void Construct();

private:
  /// No default constructor.
  BDSParallelWorldSampler() = delete;
  
  /// Cache of the placements to clean up at the end.
  std::vector<G4VPhysicalVolume*> placements;

  /// Cache of samplers to delete at the end
  std::vector<BDSSampler*> samplers;

  /// Just the input part of the world name.
  G4String suffix;

  /// Visualisation attributes for the sampler world.
  G4VisAttributes* samplerWorldVis;
};

#endif
