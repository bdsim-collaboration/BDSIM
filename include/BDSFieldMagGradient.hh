#ifndef BDSFIELDMAGGRADIENT_H
#define BDSFIELDMAGGRADIENT_H


#include "globals.hh"

#include <vector>

class BDSFieldMag;
class BDSMagnetStrength;
class BDSFieldMagSkew;

/**
 *Finds multipole components of fieldmaps by numerical differentiation. Not optimised right now.
 *Uses Central difference method with each level taking the previous as the function (i.e. 1st order takes b, 2nd order takes 1st)
 *Step size works right now but dropping it too low introduces large, inconsistent errors.
 *
 *
 * @author Josh Albrecht
 */
class BDSFieldMagGradient
{
public:

  BDSFieldMagGradient();

  //Find the Magnet strengths of a multipole field to nth order.
  BDSMagnetStrength* CalculateMultipoles(BDSFieldMag* BField, G4int order, G4double Brho=4);
  //Query a point on x axis for By
  G4double GetBy(BDSFieldMag* BField, G4double l, G4double h=0) const;
  //Find d(By)/dx
  G4double FirstDerivative(BDSFieldMag* BField, G4double x, G4double h);
  //Find d^2(By)/dx^2
  G4double SecondDerivative(BDSFieldMag* BField, G4double x, G4double h);

  //Find d^3(By)/dx^3
  G4double ThirdDerivative(BDSFieldMag* BField, G4double x, G4double h);
  
  //Find d^4(By)/dx^4
  G4double FourthDerivative(BDSFieldMag* BField, G4double x, G4double h);

  //Find d^5(By)/dx^5
  G4double FifthDerivative(BDSFieldMag* BField, G4double x, G4double h);

  std::vector<G4double> PrepareValues(BDSFieldMag* field,
				      G4int        order,
				      G4double     centreX,
				      G4double     h,
				      G4int&       centreIndex) const;
  
  G4double Derivative(const std::vector<G4double>& data,
		      const G4int                  order,
		      const G4int                  startIndex,
		      const G4double               h) const;

};

#endif
