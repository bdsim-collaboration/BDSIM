#ifndef BDSFIELDSKEW_H
#define BDSFIELDSKEW_H

#include "BDSFieldMag.hh"

#include "globals.hh" // geant4 types / globals
#include "G4RotationMatrix.hh"

/**
 * @brief A wrapper class for BDSFieldMag that rotates it.
 * 
 * This is a wrapper class for a BDSFieldMag instance that rotates it by applying
 * a rotation to the coordinates before querying the wrapped field object.
 * This is intended to implement skew fields but any arbritary rotation can
 * be applied, although this should be considered carefully.
 * 
 * This class owns the field and rotation matrix.
 *
 * @author Laurie Nevay
 */

class BDSFieldMagSkew: public BDSFieldMag
{
public:
  /// Wrap a field and rotate it by angle (rad) in the x-y plane (about z axis).
  BDSFieldMagSkew(BDSFieldMag* fieldIn, G4double angle = 0);

  virtual ~BDSFieldMagSkew();

  /// Get the field - local coordinates, and rotated.
  virtual G4ThreeVector GetField(const G4ThreeVector &position) const;
  
private:
  /// Private default constructor to force use of supplied ones.
  BDSFieldMagSkew();

  /// The nominal non-skewed field.
  BDSFieldMag* const field;

  /// The rotation matrix used to rotation the coordinates.
  G4RotationMatrix* rotation;

  /// The opposite rotation matrix used to transform the resultant field vector.
  G4RotationMatrix* antiRotation;

};

#endif
