#ifndef BDSINTERPOLATE4DLINEAR_H
#define BDSINTERPOLATE4DLINEAR_H

#include "BDSFieldValue.hh"
#include "BDSInterpolator4D.hh"

#include "G4Types.hh"

class BDSArray4DCoords;

/** 
 * @brief Linear interpolation over 4d array.
 * 
 * Does not own array - so multiple interpolators could be used on same data.
 * 
 * This uses a quad-linear algorithm to interpolate a 3d array and return
 * the value at any arbitrary point. If the point lies outside the array
 * the default value for the templated parameter is returned (typically 0).
 * Therefore, the field drops to 0 outside the specified region.
 * 
 * This is based on an extension of the trilinear algorithm described in 
 * BDSInterpolator3DLinear.hh.
 * 
 * @author Laurie Nevay
 */

class BDSInterpolator4DLinear: public BDSInterpolator4D
{
public:
  BDSInterpolator4DLinear(BDSArray4DCoords* arrayIn);
  virtual ~BDSInterpolator4DLinear();

protected:
  virtual BDSFieldValue GetInterpolatedValueT(G4double x,
					      G4double y,
					      G4double z,
					      G4double t) const;

private:
  /// Private default constructor to force use of provided one.
  BDSInterpolator4DLinear() = delete;
};

#endif
