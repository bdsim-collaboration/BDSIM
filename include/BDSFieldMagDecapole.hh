#ifndef BDSFIELDMAGDECAPOLE_H
#define BDSFIELDMAGDECAPOLE_H

#include "BDSFieldMag.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

class BDSMagnetStrength;

/**
 * @brief Class that provides the magnetic strength in a decapole.
 * 
 * The magnetic field is calculated from the decapole strength parameter
 * "k4" and a design rigidity (brho).
 */

class BDSFieldMagDecapole: public BDSFieldMag
{
public:
  BDSFieldMagDecapole(BDSMagnetStrength const* strength,
		      G4double          const  brho);
  
  virtual ~BDSFieldMagDecapole(){;}

  /// Access the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector &position,
				 const G4double       t = 0) const;
  
private:
  /// Private default constructor to force use of supplied constructor.
  BDSFieldMagDecapole();

  /// B'''' - the fourth derivative of the magnetic field.
  G4double bQuadruplePrime;

  /// B'''' / 4!
  G4double bQPNormed;
};

#endif
