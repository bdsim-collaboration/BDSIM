#ifndef BDSFIELDMAGQUADRUPOLE_H
#define BDSFIELDMAGQUADRUPOLE_H

#include "BDSField.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

class BDSMagnetStrength;

/**
 * @brief Class that provides the magnetic strength in a quadrupole.
 * 
 * The magnetic field is calculated from the quadrupole strength parameter
 * "k1" and a design rigidity (brho).
 */

class BDSFieldMagQuadrupole: public BDSField
{
public:
  BDSFieldMagQuadrupole(const BDSMagnetStrength* strength,
			const G4double           brho);
  
  virtual ~BDSFieldMagQuadrupole(){;}

  /// Access the field value.
  virtual G4ThreeVector GetFieldValue(const G4ThreeVector& position) const;

  /// Get the name of the field.
  virtual inline G4String Name() const;
  
private:
  /// Private default constructor to force use of supplied constructor.
  BDSFieldMagQuadrupole();
  
  /// B' - the field gradient - a constant for a quadrupole.
  G4double bPrime;
};

inline G4String BDSFieldMagQuadrupole::Name() const
{return "quadrupole";}

#endif
