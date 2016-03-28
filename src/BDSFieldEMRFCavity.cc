#include "BDSFieldEMRFCavity.hh"

#include "CLHEP/Units/PhysicalConstants.h" 
#include "globals.hh"
#include "G4ThreeVector.hh"

#include <cmath>
#include <utility>

BDSFieldEMRFCavity::BDSFieldEMRFCavity(G4double eFieldMaxIn,
				       G4double cavityRadiusIn,
				       G4double frequencyIn,
				       G4double phaseIn):
  eFieldMax(eFieldMaxIn),
  cavityRadius(cavityRadiusIn),
  frequency(frequencyIn),
  phase(phaseIn)
{;}

G4bool BDSFieldEMRFCavity::DoesFieldChangeEnergy() const
{
  return true;
}

std::pair<G4ThreeVector, G4ThreeVector> BDSFieldEMRFCavity::GetField(const G4ThreeVector &position,
                                                                     const G4double t) const
{
  G4double radialDistance = pow(pow(position.x(),2) +  pow(position.y(),2), 0.5);

  //Calculating the magnitudes of the Fields:  
  //J0(2.405*z/cavityRadius) ..  Uses polynomial approximation suitable for J0 arg between 0 and 3 
  double J0r = 0.999999999
    -2.249999879*pow((2.405*radialDistance/(3*cavityRadius)),2)
    +1.265623060*pow((2.405*radialDistance/(3*cavityRadius)),4)
    -0.316394552*pow((2.405*radialDistance/(3*cavityRadius)),6)
    +0.044460948*pow((2.405*radialDistance/(3*cavityRadius)),8)
    -0.003954479*pow((2.405*radialDistance/(3*cavityRadius)),10)
    +0.000212950*pow((2.405*radialDistance/(3*cavityRadius)),12); 

  //J0(2.405*z/cavityRadius) ..  Uses polynomial approximation suitable for J1 arg between 0 and 3 
  double J1r = (2.405*radialDistance/(3*cavityRadius))*
    (0.500000000
     -0.562499992*pow((2.405*radialDistance/(3*cavityRadius)),2)
     +0.210937377*pow((2.405*radialDistance/(3*cavityRadius)),4)
     -0.039550040*pow((2.405*radialDistance/(3*cavityRadius)),6)
     +0.004447331*pow((2.405*radialDistance/(3*cavityRadius)),8)
     -0.000330547*pow((2.405*radialDistance/(3*cavityRadius)),10)
     +0.000015525*pow((2.405*radialDistance/(3*cavityRadius)),12));

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
