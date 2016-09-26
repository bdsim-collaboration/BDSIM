#ifndef BDSFIELDMAGINTERPOLATED3D_H
#define BDSFIELDMAGINTERPOLATED3D_H

#include "BDSFieldMag.hh"
#include "BDSInterpolator3D.hh"

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

/**
 * @brief A 3D field from an interpolated array with any interpolation.
 *
 * This provides a simple interface for magnetic fields to use a 3D
 * interpolator irrespective of which type of interpolator it is.
 *
 * @author Laurie Nevay
 */

class BDSFieldMagInterpolated3D: public BDSFieldMag
{
public:
  BDSFieldMagInterpolated3D(BDSInterpolator3D* interpolatorIn,
			    G4Transform3D      offset = G4Transform3D::Identity):
    BDSFieldMag(offset),
    interpolator(*interpolatorIn)
  {;}

  virtual ~BDSFieldMagInterpolated3D();

  /// Return the interpolated field value at a given point. Note this doesn't depend
  /// on the z position, only x & y.
  virtual G4ThreeVector GetField(const G4ThreeVector& position) const
  {return interpolator.GetInterpolatedValueV(position[0], position[1], position[2]);}

private:
  /// Private default constructor to force use of provided one.
  BDSFieldMagInterpolated3D();

  BDSInterpolator3D* interpolator;
};

#endif
