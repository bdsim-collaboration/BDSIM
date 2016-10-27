#ifndef BDSFIELDEMINTERPOLATED1D_H
#define BDSFIELDEMINTERPOLATED1D_H

#include "BDSFieldEM.hh"

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include <utility>

class BDSInterpolator1D;

/**
 * @brief A 1D field from an interpolated array with any interpolation.
 *
 * Owns interpolators.
 *
 * This provides a simple interface for em fields to use a 1D
 * interpolator irrespective of which type of interpolator it is.
 *
 * @author Laurie Nevay
 */

class BDSFieldEMInterpolated1D: public BDSFieldEM
{
public:
  BDSFieldEMInterpolated1D(BDSInterpolator1D* eInterpolatorIn,
			   BDSInterpolator1D* bInterpolatorIn,
  			   G4Transform3D      offset = G4Transform3D::Identity);

  virtual ~BDSFieldEMInterpolated1D();

  /// Return the interpolated field value at a given point.
  virtual std::pair<G4ThreeVector,G4ThreeVector> GetField(const G4ThreeVector& position,
							  const G4double       t = 0) const;

  inline const BDSInterpolator1D* EInterpolator() const {return eInterpolator;}
  inline const BDSInterpolator1D* BInterpolator() const {return bInterpolator;}

private:
  /// Private default constructor to force use of provided one.
  BDSFieldEMInterpolated1D() = delete;

  BDSInterpolator1D* eInterpolator;
  BDSInterpolator1D* bInterpolator;
};

#endif
