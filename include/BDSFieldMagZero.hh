#ifndef BDSFIELDMAGZERO_H
#define BDSFIELDMAGZERO_H

#include "BDSFieldMag.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

/**
 * @brief Null B field - for special cases where we need a valid object.
 * 
 * Simply returns 0,0,0,0,0,0 for all field components.  Used for teleporter
 * or specifically written tracking routines that may ignore the 'field' but
 * still need a valid object that can be called by Geant4 in order to fit
 * into its scheme.
 *
 * @author Laurie Nevay.
 */

class BDSFieldMagZero: public BDSFieldMag
{
public:
  BDSFieldMagZero(){;}
  
  virtual ~BDSFieldMagZero(){;}

  /// Access the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector& /*position*/,
				 const G4double       /*t*/ = 0) const
  {return G4ThreeVector(0,0,0);}
};

#endif
