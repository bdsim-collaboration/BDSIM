#ifndef BDSFIELDEMINTERPOLATED3D_H
#define BDSFIELDEMINTERPOLATED3D_H

#include "BDSFieldEMInterpolated.hh"

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include <utility>

class BDSInterpolator3D;

/**
 * @brief A 3D field from an interpolated array with any interpolation.
 *
 * Owns interpolators.
 *
 * This provides a simple interface for em fields to use a 3D
 * interpolator irrespective of which type of interpolator it is.
 *
 * @author Laurie Nevay
 */

class BDSFieldEMInterpolated3D: public BDSFieldEMInterpolated
{
public:
  BDSFieldEMInterpolated3D(BDSInterpolator3D* eInterpolatorIn,
			   BDSInterpolator3D* bInterpolatorIn,
  			   G4Transform3D      offset     = G4Transform3D::Identity,
			   G4double           eScalingIn = 1.0,
			   G4double           bScalingIn = 1.0);

  virtual ~BDSFieldEMInterpolated3D();

  /// Return the interpolated field value at a given point.
  virtual std::pair<G4ThreeVector,G4ThreeVector> GetField(const G4ThreeVector& position,
							  const G4double       t = 0) const;

  inline const BDSInterpolator3D* EInterpolator() const {return eInterpolator;}
  inline const BDSInterpolator3D* BInterpolator() const {return bInterpolator;}

private:
  /// Private default constructor to force use of provided one.
  BDSFieldEMInterpolated3D() = delete;

  BDSInterpolator3D* eInterpolator; ///< E Interplator the field is based on.
  BDSInterpolator3D* bInterpolator; ///< B Interpolator the field is based on.
};

#endif
