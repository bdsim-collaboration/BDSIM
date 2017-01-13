#ifndef BDSFIELDMAGINTERPOLATED3D_H
#define BDSFIELDMAGINTERPOLATED3D_H

#include "BDSFieldMag.hh"

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

class BDSInterpolator3D;

/**
 * @brief A 3D field from an interpolated array with any interpolation.
 *
 * Owns interpolator.
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
			    G4Transform3D      offset    = G4Transform3D::Identity,
			    G4double           scalingIn = 1.0);

  virtual ~BDSFieldMagInterpolated3D();

  /// Return the interpolated field value at a given point.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
				 const G4double       t = 0) const;

  inline const BDSInterpolator3D* Interpolator() const {return interpolator;}

private:
  /// Private default constructor to force use of provided one.
  BDSFieldMagInterpolated3D() = delete;

  BDSInterpolator3D* interpolator; ///< Interpolator the field is based on.
  G4double           scaling;      ///< Field value scaling.
};

#endif
