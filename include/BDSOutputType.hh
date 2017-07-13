#ifndef BDSOUTPUTTYPE_H
#define BDSOUTPUTTYPE_H 

#include "BDSTypeSafeEnum.hh"
#include "globals.hh" // geant4 types / globals

/**
 * @brief Type definition for all output formats.
 */

struct outputformats_def {
  enum type {none, rootevent};
};

typedef BDSTypeSafeEnum<outputformats_def, int> BDSOutputType;

namespace BDS {
  /// Determine the output format to be used from the input string.
  BDSOutputType DetermineOutputType(G4String outputType);
}


#endif
