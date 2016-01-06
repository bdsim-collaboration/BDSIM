#ifndef BDSRANDOM_H
#define BDSRANDOM_H

#include "globals.hh" //for G4string etc

namespace BDSRandom
{
  void CreateRandomNumberGenerator();
  void SetSeed();
  void PrintFullSeedState();
  void WriteSeedState();
  void LoadSeedState (G4String inSeedFilename);
}

#endif
