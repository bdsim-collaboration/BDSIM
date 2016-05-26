#ifndef BDSFIELDMAGDETECTORSOLENOID_H
#define BDSFIELDMAGDETECTORSOLENOID_H

#include "globals.hh" // geant4 types / globals

#include "BDSFieldMag.hh"

/**
 * @brief Field with different inner and an outer solenoid field. 
 */

class BDSFieldMagDetectorSolenoid: public BDSFieldMag
{
public: 
  BDSFieldMagDetectorSolenoid(G4double BIn,
			      G4double Bout,
			      G4double radiusIn,
			      G4double radiusOut,
			      G4double zMin,
			      G4double zMax);
  
  virtual ~BDSFieldMagDetectorSolenoid(){;}

  /// Get the magnetic field vector in local coordinates.
  virtual G4ThreeVector GetField(const G4ThreeVector& position) const;
  
private:
  G4double itsBIn;
  G4double itsBOut;
  G4double itsRadiusIn;
  G4double itsRadiusOut;
  G4double itsZMin;
  G4double itsZMax;
};

#endif
