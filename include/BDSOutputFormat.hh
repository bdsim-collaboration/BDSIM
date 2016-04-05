#ifndef BDSOutputFormat_h
#define BDSOutputFormat_h 

#include "BDSTypeSafeEnum.hh"
#include "globals.hh" // geant4 types / globals

/**
 * @brief Type definition for all output formats.
 */

struct outputformats_def {
  enum type {none, ascii, root, rootdouble, rootdetailed,
	     rootdetaileddouble, rootevent, combined};
};

typedef BDSTypeSafeEnum<outputformats_def, int> BDSOutputFormat;

namespace BDS {
  /// Determine the output format to be used from the input string.
  BDSOutputFormat DetermineOutputFormat(G4String outputFormat);
}


#endif
