#ifndef BDSFIELDMAGMULTIPOLE_H
#define BDSFIELDMAGMULTIPOLE_H

#include "globals.hh" // geant4 types / globals

#include "BDSFieldMagBase.hh"

#include <vector>

class BDSMagnetStrength;

/**
 * @brief Class that provides the magnetic strength in a quadrupole.
 * 
 * The magnetic field is calculated from the strength parameters
 * "kn" up to a specified order and a design rigidity (brho).
 */

class BDSFieldMagMultipole: public BDSFieldMagBase
{
public:
  BDSFieldMagMultipole(const BDSMagnetStrength* strength,
		       const G4double           brho,
		       const G4int              orderIn = 12);
  
  ~BDSFieldMagMultipole(){;}

  /// Overridden method from G4MagneticField - access the field from this multipole
  /// at a given global set of coordinates. These are first converted to local coordinates
  /// using the cached transform from BDSAuxiliaryNavigator.
  virtual void  GetFieldValue(const G4double point[4],
			      G4double* field) const;

private:
  /// Private default constructor to force use of supplied constructor.
  BDSFieldMagMultipole();

  /// Order up to which field components are considered.
  const G4int order;

  /// Normal field components (normal - ie not skew) = kn * brho
  std::vector<G4double> normalComponents;

  /// Skew field components = kns * brho
  std::vector<G4double> skewComponents;
};


#endif 
