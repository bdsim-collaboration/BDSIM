#ifndef BDSINTERPOLATE2DCUBIC_H
#define BDSINTERPOLATE2DCUBIC_H

#include "BDSFieldValue.hh"
#include "BDSInterpolator2D.hh"

#include "G4Types.hh"

class BDSArray2DCoords;

/** 
 * @brief Cubic interpolation over 2d array.
 * 
 * Does not own array - so multiple interpolators could be used on same data.
 * 
 * This uses a bicubic algorithm to interpolate a 2d array and return
 * the value at any arbitrary point. If the point lies outside the array
 * the default value for the templated parameter is returned (typically 0).
 * Therefore, the field drops to 0 outside the specified region.
 * 
 * @author Laurie Nevay
 */

class BDSInterpolator2DCubic: public BDSInterpolator2D
{
public:
  BDSInterpolator2DCubic(BDSArray2DCoords* arrayIn);
  virtual ~BDSInterpolator2DCubic();

protected:
  virtual BDSFieldValue GetInterpolatedValueT(G4double x, G4double y) const;

private:
  /// Private default constructor to force use of provided one.
  BDSInterpolator2DCubic() = delete;

  /// Cubic interpolation in 1 dimension.
  BDSFieldValue CubicInterpolate(const BDSFieldValue p[4],
				 const G4double      x) const;

  /// Cubic interpolation in 2 dimensions.
  BDSFieldValue BiCubicInterpolate(const BDSFieldValue p[4][4],
				   const G4double      x,
				   const G4double      y) const;

  /// Cubic interpolation in 3 dimensions.
  BDSFieldValue TriCubicInterpolate(const BDSFieldValue p[4][4][4],
				    const G4double      x,
				    const G4double      y,
				    const G4double      z) const;
};

#endif
