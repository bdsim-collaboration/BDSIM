#ifndef BDSINTEGRATORDRIFT_H
#define BDSINTEGRATORDRIFT_H

#include "globals.hh"
#include "G4ThreeVector.hh"

/**
 * @brief Routine for drift algorithm.
 *
 * q_{out} = q_{in} + \hat{p}_{in} h
 *
 * @author Laurie nevay
 */

class BDSIntegratorDrift
{
public:
  BDSIntegratorDrift();

  ~BDSIntegratorDrift();

  /// Advance a particle by using a drift algorithm. Uses first 6 variables in
  /// array and assumes at least 6 elements in each array so only suitable
  /// for magnetic field integrators. Sets error to 0. Does not provide
  /// distance from chord parameter as this is by definition 0 for a drift
  /// and should be set elsewhere.
  void AdvanceDriftMag(const G4double yIn[],  ///< Input coordinates x,y,z,px,py,pz,t
		       const G4double h,      ///< Step length in mm
		       G4double       yOut[], ///< Output array [6]
		       G4double*      yErr = nullptr) const;  ///< (optional) Error array [6] all 0.

  void AdvanceDriftMag(const G4double       yIn[],
		       const G4ThreeVector& unitMomentum,
		       const G4double       h,
		       G4double             yOut[],
		       G4double*            yErr = nullptr) const;

};

#endif
