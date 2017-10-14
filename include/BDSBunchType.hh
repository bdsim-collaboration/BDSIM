#ifndef BDSBUNCHTYPE_H
#define BDSBUNCHTYPE_H

#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types

/**
 * @brief Type definition for bunch distributions - used for comparison
 * in factory methods
 * 
 * @author Laurie Nevay
 */

struct bunchtypes_def
{
  enum type {reference, gaussian, square, circle, ring, eshell,
	     sixtrack, twiss, halo, userfile, composite, ptc};
};

typedef BDSTypeSafeEnum<bunchtypes_def,int> BDSBunchType;

namespace BDS
{
  /// Function that gives corresponding enum value for string (case-insensitive).
  BDSBunchType DetermineBunchType(G4String distrType);
}

#endif
