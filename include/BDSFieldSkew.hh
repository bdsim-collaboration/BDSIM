#ifndef BDSFIELDSKEW_H
#define BDSFIELDSKEW_H

#include "BDSField.hh"

#include "globals.hh" // geant4 types / globals
#include "G4RotationMatrix.hh"

/**
 * @brief A wrapper class for BDSField that rotates it.
 * 
 * This is a wrapper class for a BDSFieldInstance that rotates it by applying
 * a rotation to the coordinates before querying the wrapped field object.
 * This is intended to implement skew fields but any arbritary rotation can
 * be applied, although this should be considered carefully.
 * 
 * This class owns the field and rotation matrix.
 *
 * @author Laurie Nevay
 */

class BDSFieldSkew: public BDSField
{
public:
  /// Wrap a field and rotate it by angle (rad) in the x-y plane (about z axis).
  BDSFieldSkew(BDSField* fieldIn, G4double angle = 0);

  /// Wrap a field and rotate it according to a rotation matrix.
  BDSFieldSkew(BDSField* fieldIn, G4RotationMatrix* rotationIn);

  virtual ~BDSFieldSkew();

  /// Get the field - local coordinates, and rotated.
  virtual G4ThreeVector GetFieldValue(const G4ThreeVector& position) const;

  /// Name of field.
  inline virtual G4String Name() const;
  
private:
  /// Private default constructor to force use of supplied ones.
  BDSFieldSkew();

  /// The nominal non-skewed field.
  BDSField* const field;

  /// The rotation matrix used to rotation the coordinates.
  G4RotationMatrix* rotation;

};

inline G4String BDSFieldSkew::Name() const
{
  G4String name = "skew " + field->Name();
  return name;
}

#endif
