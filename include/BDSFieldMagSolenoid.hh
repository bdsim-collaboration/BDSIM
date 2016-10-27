#ifndef BDSFIELDMAGSOLENOID_H
#define BDSFIELDMAGSOLENOID_H

#include "BDSFieldMag.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

class BDSMagnetStrength;

/**
 * @brief Class the provides the field in a solenoid.
 * 
 * This simply returns a uniform field that is independent of
 * position.
 * 
 * @author Laurie Nevay
 */

class BDSFieldMagSolenoid: public BDSFieldMag
{
public:
  BDSFieldMagSolenoid(BDSMagnetStrength const* strength,
		      G4double          const  brho);
  
  virtual ~BDSFieldMagSolenoid(){;}

  /// Access the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector &position,
				 const G4double       t = 0) const;
  
private:
  /// Private default constructor to ensure use of supplied constructor
  BDSFieldMagSolenoid();

  /// The value of the uniform magnetic field in local coordinates.
  G4ThreeVector localField;
};

#endif
