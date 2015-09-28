#ifndef BDSBTYPE_H
#define BDSBTYPE_H

#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types

/**
 * @brief Type definition for b field formats - 
 * used for comparison in factory methods.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

struct btypes_def
{
  enum type {zero, threed, xy, mokka};
};

typedef BDSTypeSafeEnum<btypes_def,int> BDSBType;

namespace BDS
{
  /// function that gives corresponding enum value for string (case-insensitive)
  BDSBType DetermineBType(G4String bType);
}

#endif


