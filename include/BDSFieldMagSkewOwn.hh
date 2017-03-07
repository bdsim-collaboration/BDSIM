#ifndef BDSFIELDSKEWOWN_H
#define BDSFIELDSKEWOWN_H

#include "BDSFieldMagSkew.hh"

#include "globals.hh" // geant4 types / globals

class BDSFieldMag;

/**
 * @brief A wrapper class for BDSFieldMagSkew where it owns the field.
 * 
 * @author Laurie Nevay
 */

class BDSFieldMagSkewOwn: public BDSFieldMagSkew
{
public:
  /// Wrap a field and rotate it by angle (rad) in the x-y plane (about z axis).
  BDSFieldMagSkewOwn(BDSFieldMag* fieldIn, G4double angle = 0);

  virtual ~BDSFieldMagSkewOwn();
  
private:
  /// Private default constructor to force use of supplied ones.
  BDSFieldMagSkewOwn() = delete;

  /// The nominal non-skewed field.
  BDSFieldMag* const field;
};

#endif
