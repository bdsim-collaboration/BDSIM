#ifndef BDSFIELDMAGSEXTUPOLE_H
#define BDSFIELDMAGSEXTUPOLE_H

#include "BDSField.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

class BDSMagnetStrength;

/**
 * @brief Class that provides the magnetic strength in a sextupole.
 * 
 * The magnetic field is calculated from the sextupole strength parameter
 * "k2" and a design rigidity (brho).
 */

class BDSFieldMagSextupole: public BDSField
{
public:
  BDSFieldMagSextupole(const BDSMagnetStrength* strength,
		       const G4double           brho);
  
  virtual ~BDSFieldMagSextupole(){;}

  virtual G4ThreeVector GetFieldValue(const G4ThreeVector& position) const;
  
private:
  /// Private default constructor to avoid usage.
  BDSFieldMagSextupole(){;}
  
  /// B'' - second derivative of the magnetic field.
  G4double bDoublePrime;
  
  /// bDoublePrime / 2! cached
  G4double halfBDoublePrime;
};

#endif
