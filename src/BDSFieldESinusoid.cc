#include "BDSCavityInfo.hh"
#include "BDSFieldESinusoid.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

#include <cmath>

BDSFieldESinusoid::BDSFieldESinusoid(BDSMagnetStrength const* strength):
  BDSFieldESinusoid((*strength)["eField"],
		    (*strength)["frequency"],
		    (*strength)["phaseOffset"])
{;}

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
  G4ThreeVector field = G4ThreeVector(0, 0, eField*sin(frequency*t + phase));
  return field;
}
