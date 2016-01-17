#ifndef BDSFIELDMAGMULTIPOLE_H
#define BDSFIELDMAGMULTIPOLE_H

#include "BDSField.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include <vector>

class BDSMagnetStrength;

/**
 * @brief Class that provides the magnetic strength in a quadrupole.
 * 
 * The magnetic field is calculated from the strength parameters
 * "kn" up to a specified order and a design rigidity (brho).
 */

class BDSFieldMagMultipole: public BDSField
{
public:
  BDSFieldMagMultipole(const BDSMagnetStrength* strength,
		       const G4double           brho,
		       const G4int              orderIn = 12);
  
  virtual ~BDSFieldMagMultipole(){;}

  /// Access the field value.
  virtual G4ThreeVector GetFieldValue(const G4ThreeVector& position) const;

  /// Get the name of the field.
  virtual inline G4String Name() const;

private:
  /// Private default constructor to force use of supplied constructor.
  BDSFieldMagMultipole();

  /// Order up to which field components are considered.
  G4int order;

  /// Normal field components (normal - ie not skew) = kn * brho
  std::vector<G4double> normalComponents;

  /// Skew field components = kns * brho
  std::vector<G4double> skewComponents;
};

inline G4String BDSFieldMagMultipole::Name() const
{return "mulitpole";}

#endif 
