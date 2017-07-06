#include "BDSCavityInfo.hh"
#include "BDSFieldEMRFCavity.hh"
#include "BDSMagnetStrength.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "globals.hh"
#include "G4ThreeVector.hh"

#include "TMath.h"

#include <cmath>
#include <utility>

const G4double BDSFieldEMRFCavity::j0FirstZero = 2.404825557695772768622;

const G4double BDSFieldEMRFCavity::Z0 = CLHEP::mu0 * CLHEP::c_light;

BDSFieldEMRFCavity::BDSFieldEMRFCavity(BDSMagnetStrength const* strength):
  BDSFieldEMRFCavity((*strength)["eField"],
		     (*strength)["frequency"],
		     (*strength)["phase"],
		     (*strength)["cavityRadius"])
{;}

BDSFieldEMRFCavity::BDSFieldEMRFCavity(G4double eFieldAmplitude,
				       G4double frequencyIn,
				       G4double phaseOffset,
				       G4double cavityRadiusIn):
  eFieldMax(eFieldAmplitude),
  frequency(frequencyIn),
  phase(phaseOffset),
  cavityRadius(cavityRadiusIn),
  normalisedCavityRadius(j0FirstZero/cavityRadius)
{;}

std::pair<G4ThreeVector, G4ThreeVector> BDSFieldEMRFCavity::GetField(const G4ThreeVector &position,
                                                                     const G4double       t) const
{

  //Converting from Local Cartesian to Local Cylindrical
  G4double phi = atan2(position.y(),position.x());
  G4double r   = std::hypot(position.x(),position.y());

  G4double rNormalised = normalisedCavityRadius * r;

  // In case a point outside the cavity is queried, ensure the bessel will return 0
  if (rNormalised > j0FirstZero)
    {rNormalised = j0FirstZero - 1e-6;}

  //Source for calculating the TM010 mode: Gerigk, Frank. "Cavity types." arXiv preprint arXiv:1111.4897 (2011).

  G4double J0r = TMath::BesselJ0(rNormalised);
  G4double J1r = TMath::BesselJ1(rNormalised);

  // Calculating free-space impedance and scale factor for Bphi:
  G4double hMax = -eFieldMax/Z0;
  G4double Bmax = hMax * CLHEP::mu0;

  //Calculating field components.  Frequency in rad/s or /s?
  G4double Ez = eFieldMax * J0r * cos(frequency*t);
  G4double Bphi = Bmax * J1r * sin(frequency*t);

  //Converting Bphi into cartesian coordinates:

  G4double Bx = Bphi*sin(phi);
  G4double By = Bphi*cos(phi);

  //Local B and E fields:
  G4ThreeVector LocalB = G4ThreeVector(Bx,                          //x
				       By,                          //y
				       0);                          //z

  G4ThreeVector LocalE = G4ThreeVector(0,                           //x
				       0,                           //y
				       Ez);                         //z

  auto result = std::make_pair(LocalB, LocalE);
  return result;
}
