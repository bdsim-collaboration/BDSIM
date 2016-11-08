#ifndef BDSFIELDEMINTERPOLATED2D_H
#define BDSFIELDEMINTERPOLATED2D_H

#include "BDSFieldEM.hh"

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include <utility>

class BDSInterpolator2D;

/**
 * @brief A 2D field from an interpolated array with any interpolation.
 *
 * Owns interpolators.
 *
 * This provides a simple interface for em fields to use a 2D
 * interpolator irrespective of which type of interpolator it is.
 *
 * @author Laurie Nevay
 */

class BDSFieldEMInterpolated2D: public BDSFieldEM
{
public:
  BDSFieldEMInterpolated2D(BDSInterpolator2D* eInterpolatorIn,
			   BDSInterpolator2D* bInterpolatorIn,
  			   G4Transform3D      offset     = G4Transform3D::Identity,
			   G4double           eScalingIn = 1.0,
			   G4double           bScalingIn = 1.0);

  virtual ~BDSFieldEMInterpolated2D();

  /// Return the interpolated field value at a given point.
  virtual std::pair<G4ThreeVector,G4ThreeVector> GetField(const G4ThreeVector& position,
							  const G4double       t = 0) const;

  inline const BDSInterpolator2D* EInterpolator() const {return eInterpolator;}
  inline const BDSInterpolator2D* BInterpolator() const {return bInterpolator;}

private:
  /// Private default constructor to force use of provided one.
  BDSFieldEMInterpolated2D() = delete;

  BDSInterpolator2D* eInterpolator; ///< E Interpolator the field is based on.
  BDSInterpolator2D* bInterpolator; ///< B Interpolator the field is based on.
  G4double           eScaling;      ///< E Field value scaling.
  G4double           bScaling;      ///< B Field value scaling.
};

#endif
