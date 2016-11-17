#ifndef BDSFIELDEMRFCAVITY_H
#define BDSFIELDEMRFCAVITY_H

#include "BDSFieldEM.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

#include <utility>

class BDSCavityInfo;

/**
 * @brief Pill box cavity electro-magnetic field.
 *
 * @author Stuart Walker
 */

class BDSFieldEMRFCavity: public BDSFieldEM
{
public:
  BDSFieldEMRFCavity(BDSCavityInfo const* info);
  
  virtual ~BDSFieldEMRFCavity(){;}

  /// Accessor to get B and E field.
  virtual std::pair<G4ThreeVector, G4ThreeVector> GetField(const G4ThreeVector& position,
							   const G4double       t) const;
  
private:
  /// Private constructor to force use of provided one.
  BDSFieldEMRFCavity();
  
  /// Maximum field in V/m.
  G4double eFieldMax;

  /// Radius at maximum extent of cavity.
  G4double cavityRadius;
  
  /// Angular frequency.
  G4double frequency;

  /// Phase of oscillator.
  G4double phase;

  /// X coordinate of first 0 point for bessel J0.
  static const G4double j0FirstZero;

  /// Precalculated normalised calulated radius w.r.t. bessel first 0.
  const G4double normalisedCavityRadius;
};

#endif
