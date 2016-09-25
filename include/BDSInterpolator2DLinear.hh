#ifndef BDSINTERPOLATE2DLINEAR_H
#define BDSINTERPOLATE2DLINEAR_H

#include "BDSFieldData.hh"
#include "BDSInterpolator2D.hh"

#include "globals.hh" // geant4 types / globals

/** 
 * @brief Linear interpolation over 2d array.
 * 
 * Does not own array - so multiple interpolators could be used on same data.
 * 
 * This uses a bilinear algorithm to interpolate a 2d array and return
 * the value at any arbitrary point. If the point lies outside the array
 * the default value for the templated parameter is returned (typically 0).
 * Therefore, the field drops abruptly to 0 outside the specified region.
 * 
 * @author Laurie Nevay
 */

class BDSInterpolator2DLinear: public BDSInterpolator2D
{
public:
  BDSInterpolator2DLinear(BDSArray2DCoords3VF* arrayIn);
  virtual ~BDSInterpolator2DLinear();

protected:
  virtual BDSThreeVectorF GetInterpolatedValueT(G4double x, G4double y) const;

private:
  /// Private default constructor to force use of provided one.
  BDSInterpolator2DLinear() = delete;

  /// The field data
  BDSArray2DCoords3VF* array;
};

#endif
