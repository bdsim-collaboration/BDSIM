#ifndef BDSFIELDMAGSEXTUPOLE_H
#define BDSFIELDMAGSEXTUPOLE_H

#include "globals.hh" // geant4 types / globals

#include "BDSFieldMagBase.hh"

class BDSMagnetStrength;

/**
 * @brief Class that provides the magnetic strength in a sextupole.
 * 
 * The magnetic field is calculated from the sextupole strength parameter
 * "k2" and a design rigidity (brho).
 */

class BDSFieldMagSextupole: public BDSFieldMagBase
{
public:
  BDSFieldMagSextupole(const BDSMagnetStrength* strength,
		       const G4double           brho);
  
  virtual ~BDSFieldMagSextupole(){;}
  
  virtual void  GetFieldValue(const G4double point[4],
			      G4double* field) const;
  
private:
  /// Private default constructor to avoid usage.
  BDSFieldMagSextupole(){;}
  
  /// B'' - second derivative of the magnetic field.
  G4double bDoublePrime;

  /// B'' / 3! cached for simplicity
  G4double bTPNormed;
  
  /// bDoublePrime / 2 cached
  G4double halfBDoublePrime;
};

#endif
