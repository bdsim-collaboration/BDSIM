#ifndef BDSFIELDMAGDECAPOLE_H
#define BDSFIELDMAGDECAPOLE_H

#include "globals.hh" // geant4 types / globals

#include "BDSFieldMagBase.hh"

class BDSMagnetStrength;

/**
 * @brief Class that provides the magnetic strength in a decapole.
 * 
 * The magnetic field is calculated from the decapole strength parameter
 * "k4" and a design rigidity (brho).
 */

class BDSFieldMagDecapole: public BDSFieldMagBase
{
public:
  BDSFieldMagDecapole(const BDSMagnetStrength* strength,
		      const G4double           brho);
  
  ~BDSFieldMagDecapole(){;}

  /// Overridden method from G4MagneticField - access the field from this decapole
  /// at a given global set of coordinates. These are first converted to local coordinates
  /// using the cached transform from BDSAuxiliaryNavigator.
  virtual void GetFieldValue(const G4double point[4],
			     G4double* field) const;
  
private:
  /// Private default constructor to force use of supplied constructor.
  BDSFieldMagDecapole();

  /// B'''' - the fourth derivative of the magnetic field.
  G4double bQuadruplePrime;

  /// B'''' / 4!
  G4double bQPNormed;
};

#endif
