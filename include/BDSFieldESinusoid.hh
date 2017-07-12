#ifndef BDSFIELDESINUSOID_H
#define BDSFIELDESINUSOID_H

#include "BDSFieldE.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

class BDSCavityInfo;
class BDSMagnetStrength;

/**
 * @brief A sinusoidal electric (only) field that doesn't vary with position.
 * 
 * @author Laurie Nevay
 */

class BDSFieldESinusoid: public BDSFieldE
{
public:
  /// Construct with a BDSMagnetStrength instance for convenience.
  BDSFieldESinusoid(BDSMagnetStrength const* strength,
		    G4double                 brho);

  /// Construct from E amplitude, frequency (G4Units) and phase.
  BDSFieldESinusoid(G4double eFieldAmplitude,
		    G4double frequencyIn,
		    G4double phaseOffsetIn);

  virtual ~BDSFieldESinusoid(){;}

  /// Accessor for field value.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
				 const G4double       t) const;

protected:
  /// Amplitude of electric field in V/m.
  G4double eField;
  
private:
  /// Private default constructor to force use of supplied one.
  BDSFieldESinusoid() = delete;
  
  /// Angular frequency of field.
  G4double angularFrequency;

  /// Phase in radians.
  G4double phase;
};

#endif
