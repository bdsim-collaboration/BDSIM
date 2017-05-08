#ifndef BDSFIELDEMZERO_H
#define BDSFIELDEMZERO_H

#include "BDSFieldEM.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include <utility>

/**
 * @brief Null EM field - for special cases where we need a valid object.
 * 
 * Simply returns 0,0,0,0,0,0 for all field components.
 *
 * @author Laurie Nevay.
 */

class BDSFieldEMZero: public BDSFieldEM
{
public:
  BDSFieldEMZero(){;}
  
  virtual ~BDSFieldEMZero(){;}

  /// Access the field value.
  virtual std::pair<G4ThreeVector,G4ThreeVector> GetField(const G4ThreeVector& /*position*/,
				 const G4double       /*t*/ = 0) const
  {return std::make_pair(G4ThreeVector(0,0,0),G4ThreeVector(0,0,0));}
};

#endif
