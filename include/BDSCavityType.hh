#ifndef BDSCAVITYTYPE_H
#define BDSCAVITYTYPE_H

#include "globals.hh" // geant4 globals / types

#include "BDSTypeSafeEnum.hh"

#include <map>

/**
 * @brief Type definition for RF cavities
 * 
 * @author Laurie Nevay
 */

struct cavitytypes_def
{
  enum type {elliptical, rectangular, pillbox};
};

typedef BDSTypeSafeEnum<cavitytypes_def,int> BDSCavityType;

namespace BDS
{
  /// function to determine the enum type of the cavity geometry (case-insensitive)
  BDSCavityType DetermineCavityType(G4String cavityType);
}

#endif
