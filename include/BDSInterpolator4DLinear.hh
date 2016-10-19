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

  /// Interpolate between to field values for a given normalised distance between
  /// 0,1.
  BDSFieldValue Interpolate1D(const BDSFieldValue& v1,
			      const BDSFieldValue& v2,
			      const G4double       dist) const;
};

#endif
