#ifndef BDSINTERPOLATE3DCUBIC_H
#define BDSINTERPOLATE3DCUBIC_H

#include "BDSFieldValue.hh"
#include "BDSInterpolator3D.hh"

#include "G4Types.hh"

class BDSArray3DCoords;

/** 
 * @brief Cubic interpolation over 3d array.
 * 
 * Does not own array - so multiple interpolators could be used on same data.
 * 
 * This uses a tricubic algorithm to interpolate a 3d array and return
 * the value at any arbitrary point. If the point lies outside the array
 * the default value for the templated parameter is returned (typically 0).
 * Therefore, the field drops to 0 outside the specified region.
 * 
 * @author Laurie Nevay
 */

class BDSInterpolator3DCubic: public BDSInterpolator3D
{
public:
  BDSInterpolator3DCubic(BDSArray3DCoords* arrayIn);
  virtual ~BDSInterpolator3DCubic();

protected:
  virtual BDSFieldValue GetInterpolatedValueT(G4double x, G4double y, G4double z) const;

private:
  /// Private default constructor to force use of provided one.
  BDSInterpolator3DCubic() = delete;
};

#endif
