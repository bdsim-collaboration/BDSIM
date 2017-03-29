#ifndef BDSFIELDEINTERPOLATED2D_H
#define BDSFIELDEINTERPOLATED2D_H

#include "BDSFieldEInterpolated.hh"

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

class BDSInterpolator2D;

/**
 * @brief A 2D field from an interpolated array with any interpolation.
 * 
 * Owns interpolator.
 *
 * This provides a simple interface for electric fields to use a 2D
 * interpolator irrespective of which type of interpolator it is.
 *
 * @author Laurie Nevay
 */

class BDSFieldEInterpolated2D: public BDSFieldEInterpolated
{
public:
  BDSFieldEInterpolated2D(BDSInterpolator2D* interpolatorIn,
			  G4Transform3D      offset     = G4Transform3D::Identity,
			  G4double           eScalingIn = 1.0);

  virtual ~BDSFieldEInterpolated2D();

  /// Return the interpolated field value at a given point. Note this doesn't depend
  /// on the z position, only x & y.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
				 const G4double       t = 0) const;

  inline const BDSInterpolator2D* Interpolator() const {return interpolator;}

private:
  /// Private default constructor to force use of provided one.
  BDSFieldEInterpolated2D() = delete;

  BDSInterpolator2D* interpolator; ///< Interpolator the field is based on.
};

#endif
