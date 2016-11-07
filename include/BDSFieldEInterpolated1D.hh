#ifndef BDSFIELDEINTERPOLATED1D_H
#define BDSFIELDEINTERPOLATED1D_H

#include "BDSFieldE.hh"

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

class BDSInterpolator1D;

/**
 * @brief A 1D field from an interpolated array with any interpolation.
 *
 * Owns interpolator.
 *
 * This provides a simple interface for electric fields to use a 1D
 * interpolator irrespective of which type of interpolator it is.
 *
 * @author Laurie Nevay
 */

class BDSFieldEInterpolated1D: public BDSFieldE
{
public:
  BDSFieldEInterpolated1D(BDSInterpolator1D* interpolatorIn,
			  G4Transform3D      offset    = G4Transform3D::Identity,
			  G4double           scalingIn = 1.0);

  virtual ~BDSFieldEInterpolated1D();

  /// Return the interpolated field value at a given point.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
				 const G4double       t = 0) const;

  inline const BDSInterpolator1D* Interpolator() const {return interpolator;}

private:
  /// Private default constructor to force use of provided one.
  BDSFieldEInterpolated1D() = delete;

  BDSInterpolator1D* interpolator; ///< Interpolator the field is based on.
  G4double           scaling;      ///< Field value scaling.
};

#endif
