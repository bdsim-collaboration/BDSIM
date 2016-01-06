#ifndef BDSCAVITYTYPE_H
#define BDSCAVITYTYPE_H

#include "globals.hh" // geant4 globals / types

#include "BDSTypeSafeEnum.hh"

#include <map>

/**
 * @brief Type definition for RF cavities
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */


/// these must be manually duplicated in the
/// corresponding source file for this header in a string map!
struct cavitytypes_def {
  enum type {elliptical, rectangular, pillbox};
};

typedef BDSTypeSafeEnum<cavitytypes_def,int> BDSCavityType;

namespace BDS {
  /// function to determine the enum type of the cavity geometry (case-insensitive)
  BDSCavityType DetermineCavityType(G4String cavityType);
}

#endif
