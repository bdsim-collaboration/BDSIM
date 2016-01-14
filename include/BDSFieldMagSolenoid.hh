#ifndef BDSFIELDMAGSOLENOID_H
#define BDSFIELDMAGSOLENOID_H

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include "BDSFieldMagBase.hh"

class BDSMagnetStrength;

class BDSFieldMagSolenoid: public BDSFieldMagBase
{
public:
  BDSFieldMagSolenoid(BDSMagnetStrength* strength,
		      G4double           brho);
  
  ~BDSFieldMagSolenoid(){;}

  /// Get Field Value (independent of position, since solenoid field is uniform)
  virtual void  GetFieldValue(const G4double point[4],
			      G4double* field) const;
  
private:
  /// Private default constructor to ensure use of supplied constructor
  BDSFieldMagSolenoid();

  /// The value of the uniform magnetic field in local coordinates.
  G4ThreeVector localField;
};

#endif
