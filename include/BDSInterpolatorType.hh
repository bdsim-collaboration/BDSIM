#ifndef BDSINTERPOLATORTYPE_H
#define BDSINTERPOLATORTYPE_H

#include "BDSTypeSafeEnum.hh"

#include "globals.hh" // geant4 types / globals

/**
 * @brief Type definition for interpolator.
 * 
 * @author Laurie Nevay
 */

struct interpolatortypes_def
{
  enum type {none, nearest2D, linear2D, nearest3D, linear3D};
};

typedef BDSTypeSafeEnum<interpolatortypes_def,int> BDSInterpolatorType;

namespace BDS
{
  /// Function that determines enum from string (case-insensitive).
  BDSInterpolatorType DetermineInterpolatorType(G4String interpolatorType);
}

#endif
