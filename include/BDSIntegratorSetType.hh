#ifndef BDSINTEGRATORSETTYPE_H
#define BDSINTEGRATORSETTYPE_H

#include "BDSTypeSafeEnum.hh"

#include "globals.hh" // geant4 types / globals

struct integratorsettype_def
{
  enum type {geant4, bdsim};
};

typedef BDSTypeSafeEnum<integratorsettype_def, int> BDSIntegratorSetType;

namespace BDS
{
  /// Function that gives corresponding enum value for string (case-insensitive)
  BDSIntegratorSetType DetermineIntegratorSetType(G4String integratorSet);
}

#endif
