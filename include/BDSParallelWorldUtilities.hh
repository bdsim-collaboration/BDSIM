#ifndef BDSPARALLELWORLDUTILITIES_H
#define BDSPARALLELWORLDUTILITIES_H

#include "BDSParallelWorldInfo.hh"

#include "globals.hh"

#include <vector>

class G4ParallelWorldPhysics;
class G4VModularPhysicsList;
class G4VUserDetectorConstruction;
class G4VUserParallelWorld;

namespace BDS
{
  /// The number of curvilinear and sampler worlds required. This is not curvilinear +
  /// curvilinear bridge. This is over and above the one for the main beam line. Note,
  /// an extra beam line, if it exists, will require a curvilinear world, but not
  /// necessarily a sampler world.
  std::vector<BDSParallelWorldInfo> NumberOfExtraWorldsRequired();

  /// Construct the default and any extra parallel worlds required and register them
  /// to the main mass world argument. Returns the vector of sampler worlds as these
  /// are required for the parallel world physics processes.
  std::vector<G4VUserParallelWorld*> ConstructAndRegisterParallelWorlds(G4VUserDetectorConstruction* massWorld);

  /// Construct the parallel physics process for each sampler world.
  std::vector<G4ParallelWorldPhysics*> ConstructSamplerParallelPhysics(std::vector<G4VUserParallelWorld*> worlds);

  /// Register each parallel physics process to the main physics list.
  void RegisterSamplerPhysics(std::vector<G4ParallelWorldPhysics*> processes,
			      G4VModularPhysicsList* physicsList);
}

#endif
