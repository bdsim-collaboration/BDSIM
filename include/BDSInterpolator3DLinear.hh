#ifndef BDSINTERPOLATE3DLINEAR_H
#define BDSINTERPOLATE3DLINEAR_H

#include "BDSFieldValue.hh"
#include "BDSInterpolator3D.hh"

#include "G4Types.hh"

class BDSArray3DCoords;

/** 
 * @brief Linear interpolation over 3d array.
 * 
 * Does not own array - so multiple interpolators could be used on same data.
 * 
 * This uses a trilinear algorithm to interpolate a 3d array and return
 * the value at any arbitrary point. If the point lies outside the array
 * the default value for the templated parameter is returned (typically 0).
 * Therefore, the field drops to 0 outside the specified region.
 *
 * This is based on the following article:
 * https://en.wikipedia.org/wiki/Trilinear_interpolation
 * 
 * @author Laurie Nevay
 */

class BDSInterpolator3DLinear: public BDSInterpolator3D
{
public:
  BDSInterpolator3DLinear(BDSArray3DCoords* arrayIn);
  virtual ~BDSInterpolator3DLinear();

protected:
  virtual BDSFieldValue GetInterpolatedValueT(G4double x, G4double y, G4double z) const;

private:
  /// Private default constructor to force use of provided one.
  BDSInterpolator3DLinear() = delete;

  /// Interpolate between to field values for a given normalised distance between
  /// 0,1.
  BDSFieldValue Interpolate1D(const BDSFieldValue& v1,
			      const BDSFieldValue& v2,
			      const G4double       dist) const;
};

#endif
