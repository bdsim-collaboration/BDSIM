#ifndef BDSFIELDMAGOCTUPOLE_H
#define BDSFIELDMAGOCTUPOLE_H

#include "BDSFieldMag.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

class BDSMagnetStrength;

/**
 * @brief Class that provides the magnetic strength in an octupole.
 * 
 * The magnetic field is calculated from the octuupole strength parameter
 * "k3" and a design rigidity (brho).
 */

class BDSFieldMagOctupole: public BDSFieldMag
{
public:
  BDSFieldMagOctupole(BDSMagnetStrength const* strength,
		      G4double          const  brho);

  virtual ~BDSFieldMagOctupole(){;}

  /// Access the field value.
  virtual G4ThreeVector GetFieldValue(const G4ThreeVector& position) const;

  /// Get the name of the field.
  virtual inline G4String Name() const;
  
private:
  /// Private default constructor to force use of supplied constructor.
  BDSFieldMagOctupole();

  /// 3rd derivative of the magnetic field
  G4double bTriplePrime;

  /// bTriplePrime / 3! cached for simplicity
  G4double bTPNormed; 
};

inline G4String BDSFieldMagOctupole::Name() const
{return "octupole";}

#endif
