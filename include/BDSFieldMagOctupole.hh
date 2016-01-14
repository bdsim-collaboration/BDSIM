#ifndef BDSFIELDMAGOCTUPOLE_H
#define BDSFIELDMAGOCTUPOLE_H

#include "globals.hh" // geant4 types / globals

#include "BDSFieldMagBase.hh"

class BDSMagnetStrength;

/**
 * @brief Class that provides the magnetic strength in an octupole.
 * 
 * The magnetic field is calculated from the octuupole strength parameter
 * "k3" and a design rigidity (brho).
 */

class BDSFieldMagOctupole: public BDSFieldMagBase
{
public:
  BDSFieldMagOctupole(const BDSMagnetStrength* strength,
		      const G4double           brho);

  ~BDSFieldMagOctupole(){;}

  /// Overridden method from G4MagneticField - access the field from this octupole
  /// at a given global set of coordinates. These are first converted to local coordinates
  /// using the cached transform from BDSAuxiliaryNavigator.
  virtual void GetFieldValue(const G4double point[4],
			     G4double* field ) const;
  
private:
  /// Private default constructor to force use of supplied constructor.
  BDSFieldMagOctupole();

  /// 3rd derivative of the magnetic field
  G4double bTriplePrime;

  /// bTriplePrime / 3! cached for simplicity
  G4double bTPNormed; 
};

#endif
