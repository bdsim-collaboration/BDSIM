#ifndef BDSFIELDMAGSOLENOID_H
#define BDSFIELDMAGSOLENOID_H

#include "BDSField.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

class BDSMagnetStrength;

/**
 * @ brief Class the provides the field in a solenoid.
 * 
 * This simply returns a uniform field that is independent of
 * position.
 * 
 * @author Laurie Nevay
 */

class BDSFieldMagSolenoid: public BDSField
{
public:
  BDSFieldMagSolenoid(BDSMagnetStrength* strength,
		      G4double           brho);
  
  virtual ~BDSFieldMagSolenoid(){;}

  virtual G4ThreeVector GetFieldValue(const G4ThreeVector& position) const;
  
private:
  /// Private default constructor to ensure use of supplied constructor
  BDSFieldMagSolenoid();

  /// The value of the uniform magnetic field in local coordinates.
  G4ThreeVector localField;
};

#endif
