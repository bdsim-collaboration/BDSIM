#include "BDSCavityInfo.hh"
#include "BDSFieldESinusoid.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

#include <cmath>

BDSFieldESinusoid::BDSFieldESinusoid(BDSCavityInfo const* info)
{
  eField    = info->eField;
  frequency = info->frequency;
  phase     = info->phase;
}

G4ThreeVector BDSFieldESinusoid::GetField(const G4ThreeVector& /*position*/,
					  const G4double&      t) const
{
  G4ThreeVector field = G4ThreeVector(0, 0, eField*sin(frequency*t + phase));
  return field;
}
