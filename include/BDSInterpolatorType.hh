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
  enum type
    {
      none,
      nearest2d, linear2d,
      nearest3d, linear3d,
      nearest4d, linear4d
    };
};

typedef BDSTypeSafeEnum<interpolatortypes_def,int> BDSInterpolatorType;

namespace BDS
{
  /// Function that determines enum from string (case-insensitive).
  BDSInterpolatorType DetermineInterpolatorType(G4String interpolatorType);
}

#endif
