#ifndef BDSFIELDMAGQUADRUPOLE_H
#define BDSFIELDMAGQUADRUPOLE_H

#include "globals.hh" // geant4 types / globals

#include "BDSFieldMagBase.hh"

class BDSMagnetStrength;

/**
 * @brief Class that provides the magnetic strength in a quadrupole.
 * 
 * The magnetic field is calculated from the quadrupole strength parameter
 * "k1" and a design rigidity (brho).
 */

class BDSFieldMagQuadrupole: public BDSFieldMagBase
{
public:
  BDSFieldMagQuadrupole(const BDSMagnetStrength* strength,
			const G4double           brho);
  
  ~BDSQuadMagField(){;}

  /// Overridden method from G4MagneticField - access the field from this quadrupole
  /// at a given global set of coordinates. These are first converted to local coordinates
  /// using the cached transform from BDSAuxiliaryNavigator.
  virtual void GetFieldValue(const G4double point[4],
			     G4double* field) const;
  
private:
  /// Private default constructor to force use of supplied constructor.
  BDSFieldMagQuadrupole();
  
  /// B' - the field gradient - a constant for a quadrupole.
  G4double bPrime;
};

#endif
