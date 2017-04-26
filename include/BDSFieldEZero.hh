#ifndef BDSFIELDEZERO_H
#define BDSFIELDEZERO_H

#include "BDSFieldE.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

/**
 * @brief Null E field - for special cases where we need a valid object.
 * 
 * Simply returns 0,0,0,0,0,0 for all field components.
 *
 * @author Laurie Nevay.
 */

class BDSFieldEZero: public BDSFieldE
{
public:
  BDSFieldEZero(){;}
  
  virtual ~BDSFieldEZero(){;}

  /// Access the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector& /*position*/,
				 const G4double       /*t*/ = 0) const
  {return G4ThreeVector(0,0,0);}
};

#endif
