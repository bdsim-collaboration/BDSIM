#ifndef BDSRANDOM_H
#define BDSRANDOM_H

#include "globals.hh" //for G4string etc

namespace BDSRandom
{
  /// Construct and set the random engine.
  void CreateRandomNumberGenerator();

  /// Set the initial seed. Takes the seed number from
  /// BDSGlobalConstants - if negative uses the time.
  void SetSeed();

  /// Print out seed state to G4cout.
  void PrintFullSeedState();

  /// Write the seed state out to suffix + 'seedstate.txt' in cwd.
  void WriteSeedState(G4String suffix = "");

  /// Load a seedstate.txt file and restore the engine to this status.
  void LoadSeedState (G4String inSeedFilename);

  /// Set the seed state from a string.
  void SetSeedState(G4String seedState);
}

#endif
