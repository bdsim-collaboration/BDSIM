#ifndef BDSOUTPUTLOADER_H
#define BDSOUTPUTLOADER_H

#include "parser/options.h"

#include "globals.hh" // geant4 types / globals

namespace BDS
{
  /// Load a set of options from an output file.
  GMAD::Options LoadOptions(G4String filepath);

}

#endif
