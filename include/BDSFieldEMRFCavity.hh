#ifndef BDSFIELDEMRFCAVITY_H
#define BDSFIELDEMRFCAVITY_H

#include "BDSFieldEM.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

#include <utility>

class BDSCavityInfo;
class BDSMagnetStrength;

/**
 * @brief Pill box cavity electro-magnetic field.
 *
 * @author Stuart Walker
 */

class BDSFieldEMRFCavity: public BDSFieldEM
{
public:
  BDSFieldEMRFCavity(BDSMagnetStrength const* strength,
		     G4double                 brho);
  
  BDSFieldEMRFCavity(G4double eFieldAmplitude,
		     G4double frequency,
		     G4double phaseOffset,
		     G4double cavityRadius);
  
  virtual ~BDSFieldEMRFCavity(){;}

  /// Accessor to get B and E field.
  virtual std::pair<G4ThreeVector, G4ThreeVector> GetField(const G4ThreeVector& position,
							   const G4double       t) const;
  
private:
  /// Private constructor to force use of provided one.
  BDSFieldEMRFCavity();  
  
  G4double eFieldMax;    ///< Maximum field in V/m.
  G4double frequency;    ///< Angular frequency.
  G4double phase;        ///< Phase offset of the oscillator.
  G4double cavityRadius; ///< Radius at maximum extent of cavity.

  /// X coordinate of first 0 point for bessel J0.
  static const G4double j0FirstZero;

  /// Impedance of free space.
  static const G4double Z0;
  
  /// Precalculated normalised calulated radius w.r.t. bessel first 0.
  const G4double normalisedCavityRadius;

  /// Angular frequency calculated from frequency - cached to avoid repeated calculation.
  const G4double angularFrequency;
};

#endif
