#ifndef BDSFIELDMAGQUADRUPOLE_H
#define BDSFIELDMAGQUADRUPOLE_H

#include "BDSFieldMag.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

class BDSMagnetStrength;

/**
 * @brief Class that provides the magnetic strength in a quadrupole.
 * 
 * The magnetic field is calculated from the quadrupole strength parameter
 * "k1" and a design rigidity (brho).
 */

class BDSFieldMagQuadrupole: public BDSFieldMag
{
public:
  BDSFieldMagQuadrupole(BDSMagnetStrength const* strength,
			G4double          const  brho);
  
  virtual ~BDSFieldMagQuadrupole(){;}

  /// Access the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector &position) const;
  
private:
  /// Private default constructor to force use of supplied constructor.
  BDSFieldMagQuadrupole();
  
  /// B' - the field gradient - a constant for a quadrupole.
  G4double bPrime;
};

#endif
