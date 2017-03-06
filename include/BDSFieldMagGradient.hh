#ifndef BDSFIELDMAGGRADIENT_H
#define BDSFIELDMAGGRADIENT_H


#include "globals.hh"

class BDSFieldMagInterpolated2D;
class BDSMagnetStrength;

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
  BDSMagnetStrength* CalculateMultipoles(BDSFieldMagInterpolated2D* BField, G4int order);
  //Query a point on x axis for By
  G4double GetBy(BDSFieldMagInterpolated2D* BField, G4double l, G4double angle=0);
  
  //Find d(By)/dx
  G4double FirstDerivative(BDSFieldMagInterpolated2D* BField, G4double x, G4double h);

  //Find d^2(By)/dx^2
  G4double SecondDerivative(BDSFieldMagInterpolated2D* BField, G4double x, G4double h);

  //Find d^3(By)/dx^3
  G4double ThirdDerivative(BDSFieldMagInterpolated2D* BField, G4double x, G4double h);
  
  //Find d^4(By)/dx^4
  G4double FourthDerivative(BDSFieldMagInterpolated2D* BField, G4double x, G4double h);

  //Find d^5(By)/dx^5
  G4double FifthDerivative(BDSFieldMagInterpolated2D* BField, G4double x, G4double h);


};

#endif
