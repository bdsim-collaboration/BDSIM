#include "BDSFieldEMRFCavity.hh"

#include "CLHEP/Units/PhysicalConstants.h" 
#include "globals.hh"
#include "G4ThreeVector.hh"

#include "TMath.h"

#include <cmath>
#include <utility>

const G4double BDSFieldEMRFCavity::j0FirstZero = 2.404825557695772768622;

BDSFieldEMRFCavity::BDSFieldEMRFCavity(G4double eFieldMaxIn,
				       G4double cavityRadiusIn,
				       G4double frequencyIn,
				       G4double phaseIn):
  eFieldMax(eFieldMaxIn),
  cavityRadius(cavityRadiusIn),
  frequency(frequencyIn),
  phase(phaseIn)
{;}

std::pair<G4ThreeVector, G4ThreeVector> BDSFieldEMRFCavity::GetField(const G4ThreeVector &position,
                                                                     const G4double t) const
{
  G4double r = pow(pow(position.x(),2) +  pow(position.y(),2), 0.5);

  G4double rNormalised = (j0FirstZero / cavityRadius) * r;

  // In case a point outside the cavity is queried, ensure the bessel will return 0
  if (rNormalised > j0FirstZero)
    {rNormalised = j0FirstZero - 1e-6;}

  G4double J0r = TMath::BesselJ0(rNormalised);
  G4double J1r = TMath::BesselJ1(rNormalised);

  /*
  //Calculating the magnitudes of the Fields:  
  //J0(2.405*z/cavityRadius) ..  Uses polynomial approximation suitable for J0 arg between 0 and 3

  G4double factor = (2.405*r) / (3*cavityRadius);
  
  double J0r = 0.999999999
    -2.249999879*pow(factor,2)
    +1.265623060*pow(factor,4)
    -0.316394552*pow(factor,6)
    +0.044460948*pow(factor,8)
    -0.003954479*pow(factor,10)
    +0.000212950*pow(factor,12);


  //J0(2.405*z/cavityRadius) ..  Uses polynomial approximation suitable for J1 arg between 0 and 3 
  double J1r = factor*
    (0.500000000
     -0.562499992*pow(factor,2)
     +0.210937377*pow(factor,4)
     -0.039550040*pow(factor,6)
     +0.004447331*pow(factor,8)
     -0.000330547*pow(factor,10)
     +0.000015525*pow(factor,12));
  */
  //|E|
  G4double eMagnitude = eFieldMax*J0r*cos(frequency*t);
  //|H|
  G4double hMagnitude = (eFieldMax/(pow(CLHEP::mu0/CLHEP::epsilon0,0.5))*J1r*sin(frequency*t));


  //Angle of the position vector.
  //Add pi/2 so that the vector is perpendicular to position.
  G4double angle = atan2(position.y(),position.x()) + CLHEP::pi*0.5; 

  //Normalization factor
  G4double normalization = pow(tan(angle)*tan(angle)+1,0.5);

  //Local B and E fields:
  G4ThreeVector LocalB = G4ThreeVector(tan(angle)*hMagnitude/normalization, //x
				       hMagnitude/normalization,            //y
				       0);                                  //z

  G4ThreeVector LocalE = G4ThreeVector(0,                                   //x
				       0,                                   //y
				       eMagnitude);                         //z

  auto result = std::make_pair(LocalB, LocalE);
  return result;
}
