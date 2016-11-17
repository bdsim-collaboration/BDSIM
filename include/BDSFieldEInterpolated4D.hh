#ifndef BDSFIELDEINTERPOLATED4D_H
#define BDSFIELDEINTERPOLATED4D_H

#include "BDSFieldE.hh"

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4Types.hh"

class BDSInterpolator4D;

/**
 * @brief A 4D field from an interpolated array with any interpolation.
 *
 * Owns interpolator.
 *
 * This provides a simple interface for electric fields to use a 4D
 * interpolator irrespective of which type of interpolator it is.
 *
 * @author Laurie Nevay
 */

class BDSFieldEInterpolated4D: public BDSFieldE
{
public:
  BDSFieldEInterpolated4D(BDSInterpolator4D* interpolatorIn,
			  G4Transform3D      offset    = G4Transform3D::Identity,
			  G4double           scalingIn = 1.0);

  virtual ~BDSFieldEInterpolated4D();

  /// Return the interpolated field value at a given point.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
				 const G4double       t) const;

  inline const BDSInterpolator4D* Interpolator() const {return interpolator;}

private:
  /// Private default constructor to force use of provided one.
  BDSFieldEInterpolated4D() = delete;

  BDSInterpolator4D* interpolator; ///< Interpolator the field is based on.
  G4double           scaling;      ///< Field value scaling.
};

#endif
