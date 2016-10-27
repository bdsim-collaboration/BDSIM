#ifndef BDSFIELDESINUSOID_H
#define BDSFIELDESINUSOID_H

#include "BDSFieldE.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

class BDSCavityInfo;

/**
 * @brief A sinusoidal electric (only) field that doesn't vary with position.
 * 
 * @author Laurie Nevay
 */

class BDSFieldESinusoid: public BDSFieldE
{
public:
  BDSFieldESinusoid(BDSCavityInfo const* info);

  virtual ~BDSFieldESinusoid(){;}

  /// Accessor for field value.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
				 const G4double       t) const;

private:
  /// Private default constructor to force use of supplied one.
  BDSFieldESinusoid();

  /// Amplitude of electric field in V/m.
  G4double eField;

  /// Frequency of field in Hertz.
  G4double frequency;

  /// Phase in radians.
  G4double phase;
};

#endif
