#ifndef BDSINTEGRATORSETTYPE_H
#define BDSINTEGRATORSETTYPE_H

#include "BDSTypeSafeEnum.hh"

#include "globals.hh" // geant4 types / globals

/**
 * @brief Type definition for integrator sets.
 *
 * @author Laurie Nevay
 */

struct integratorsettype_def
{
  enum type {bdsim, bdsimnew, geant4exact, geant4rk4};
};

typedef BDSTypeSafeEnum<integratorsettype_def, int> BDSIntegratorSetType;

namespace BDS
{
  /// Function that gives corresponding enum value for string (case-insensitive)
  BDSIntegratorSetType DetermineIntegratorSetType(G4String integratorSet);
}

#endif
