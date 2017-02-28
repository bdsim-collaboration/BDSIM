#include "BDSCavityInfo.hh"
#include "BDSFieldEMRFCavity.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "globals.hh"
#include "G4ThreeVector.hh"

#include "TMath.h"

#include <cmath>
#include <utility>

const G4double BDSFieldEMRFCavity::j0FirstZero = 2.404825557695772768622;


BDSFieldEMRFCavity::BDSFieldEMRFCavity(BDSCavityInfo const* info):
  normalisedCavityRadius(j0FirstZero/info->equatorRadius)
{
  eFieldMax    = info->eField;
  cavityRadius = info->equatorRadius;
  frequency    = info->frequency;
  phase        = info->phase;
}

std::pair<G4ThreeVector, G4ThreeVector> BDSFieldEMRFCavity::GetField(const G4ThreeVector &position,
                                                                     const G4double       t) const
{

  //Converting from Local Cartesian to Local Cylindrical
  G4double phi = atan2(position.y(),position.x());
  G4double r = std::sqrt(std::pow(position.x(),2) +  std::pow(position.y(),2));

  G4double rNormalised = normalisedCavityRadius * r;

  // In case a point outside the cavity is queried, ensure the bessel will return 0
  if (rNormalised > j0FirstZero)
    {rNormalised = j0FirstZero - 1e-6;}


  //Source for calculating the TM010 mode: Gerigk, Frank. "Cavity types." arXiv preprint arXiv:1111.4897 (2011).

  G4double J0r = TMath::BesselJ0(rNormalised);
  G4double J1r = TMath::BesselJ1(rNormalised);

  //Calculating free-space impedance and scale factor for Bphi:
  G4double Z0 = std::sqrt(CLHEP::mu0/CLHEP::epsilon0);
  G4double Bmax = -eFieldMax/Z0;

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
