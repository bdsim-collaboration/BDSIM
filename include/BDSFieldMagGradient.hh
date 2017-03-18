#ifndef BDSFIELDMAGGRADIENT_H
#define BDSFIELDMAGGRADIENT_H

#include "globals.hh"

#include <vector>

class BDSFieldMag;
class BDSMagnetStrength;
class BDSFieldMagSkew;

/**
 * @brief Find multipole components of fieldmaps by numerical differentiation. 
 *
 * Not optimised right now.
 * Uses the Central difference method with each level taking the previous as 
 * the function (i.e. 1st order takes b, 2nd order takes 1st).
 * Step size works right now but dropping it too low introduces large, 
 * inconsistent errors.
 *
 * @author Josh Albrecht
 */

class BDSFieldMagGradient
{
public:
  BDSFieldMagGradient();
  ~BDSFieldMagGradient();

  /// Find the Magnet strengths of a multipole field to nth order.
  BDSMagnetStrength* CalculateMultipoles(BDSFieldMag* BField, G4int order, G4double Brho=4);

  /// Query a point on x axis (by default) for By.
  G4double GetBy(BDSFieldMag* BField, G4double x, G4double y=0) const;

  /// Prepare a vector of values corresponding to +- (2*order) in units of h about
  /// (spatial coordinate) centreX. centreIndex is passed by reference to write out
  /// which index represents the centre of the vector with x = centreX.
  std::vector<G4double> PrepareValues(BDSFieldMag* field,
				      G4int        order,
				      G4double     centreX,
				      G4double     h,
				      G4int&       centreIndex) const;

  /// Similar to PrepareValues but for skew fields. For each order, rotate field
  /// by CLHEP::pi / (2*order + 2) - ie quadrupole = order 1 -> pi/4. For each
  /// rotation provide the vector of values to calculate gradients on.
  std::vector<std::vector<G4double> > PrepareSkewValues(BDSFieldMag* field,
							G4int        order,
							G4double     centreX,
							G4double     h,
							G4int&       centreIndex) const;
  
  /// Returns 'order' derivative of data about startIndex in data with
  /// assumed spacing of samples of h (in Geant4 units). Derivative returned
  /// is in SI T/(m^order).
  G4double Derivative(const std::vector<G4double>& data,
		      const G4int                  order,
		      const G4int                  startIndex,
		      const G4double               h) const;

};

#endif
