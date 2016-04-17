#ifndef BDSFIELDMAGINTERPOLATED2D_H
#define BDSFIELDMAGINTERPOLATED2D_H

#include "BDSFieldMag.hh"
#include "BDSInterpolator2D.hh"

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

/**
 * @brief A 2D field from an interpolated array with any interpolation.
 *
 * This provides a simple interface for magnetic fields to use a 2D
 * interpolator irrespective of which type of interpolator it is.
 *
 * @author Laurie Nevay
 */

template<typename T>
class BDSFieldMagInterpolated2D: public BDSFieldMag
{
public:
  BDSFieldMagInterpolated2D(BDSInterpolator2D<T>* interpolatorIn,
			    G4Transform3D         offset = G4Transform3D::Identity):
    BDSFieldMag(offset), interpolator(*interpolatorIn)
  virtual ~BDSFieldMagInterpolated2D();

  /// Return the interpolated field value at a given point. Note this doesn't depend
  /// on the z position, only x & y.
  virtual G4ThreeVector GetField(const G4ThreeVector& position) const
  {return interpolator.GetInterpolatedValueV(position[0], position[1]);}

private:
  /// Private default constructor to force use of provided one.
  BDSFieldMagInterpolated2D();

  BDSInterpolator2D<T> interpolator;
};

#endif
