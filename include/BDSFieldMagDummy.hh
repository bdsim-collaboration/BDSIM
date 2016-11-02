#ifndef BDSFIELDMAGDUMMY_H
#define BDSFIELDMAGDUMMY_H

#include "globals.hh" // geant4 types / globals
#include "G4MagneticField.hh"
#include "G4ThreeVector.hh"

/**
 * @brief Null B field - for special cases where we need a valid object.
 * 
 * Simply returns 0,0,0,0,0,0 for all field components.  Used for teleporter
 * or speicially written tracking routines that may ignore the 'field' but
 * still need a valid object that can be called by Geant4 in order to fit
 * into its scheme.
 *
 * @author Laurie Nevay.
 */

class BDSFieldMagDummy: public G4MagneticField
{
public:
  BDSFieldMagDummy();
  
  virtual ~BDSFieldMagDummy(){;}

  /// Access the field value.
  virtual void GetFieldValue(const G4double point[4],
			     G4double fieldArray[6]) const;
};

#endif
