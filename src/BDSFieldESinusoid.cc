#include "BDSCavityInfo.hh"
#include "BDSFieldESinusoid.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

#include <cmath>

BDSFieldESinusoid::BDSFieldESinusoid(BDSMagnetStrength const* strength,
				     G4double                 brho):
  BDSFieldESinusoid((*strength)["eField"],
		    (*strength)["frequency"],
		    (*strength)["phase"])
{
  G4int sign = BDS::Sign(brho);
  eField *= sign;
}

BDSFieldESinusoid::BDSFieldESinusoid(G4double eFieldAmplitudeIn,
				     G4double frequencyIn,
				     G4double phaseOffsetIn):
  eField(eFieldAmplitudeIn),
  frequency(frequencyIn),
  phase(phaseOffsetIn)
{;}

G4ThreeVector BDSFieldESinusoid::GetField(const G4ThreeVector& /*position*/,
					  const G4double       t) const
{
  G4double eZ = eField*sin(frequency*t + phase);
  G4ThreeVector field = G4ThreeVector(0, 0, eZ);
  return field;
}
