#ifndef BDSINTERPOLATOR3D_H
#define BDSINTERPOLATOR3D_H

#include "BDSFieldValue.hh"

#include "G4Types.hh"
#include "G4ThreeVector.hh"

class BDSArray3DCoords;

/**
 * @brief Interface for all 3D interpolators.
 *
 * Does not own data - so multiple interpolators could be used on same data.
 *
 * @author Laurie Nevay
 */

class BDSInterpolator3D
{
public:
  BDSInterpolator3D(BDSArray3DCoords* arrayIn);
  virtual ~BDSInterpolator3D(){;}

  /// Public interface to a 3D interpolator. Returns Geant4 type as that's what will be needed.
  G4ThreeVector GetInterpolatedValue(G4double x, G4double y, G4double z) const;

  /// Alternative public interface to allow use of a 3-vector position.
  /// Returns Geant4 type as that's what will be needed.
  G4ThreeVector GetInterpolatedValue(G4ThreeVector position) const;

  inline const BDSArray3DCoords* Array() const {return array;}
  
protected:
  /// Each derived class should implement this function. Note T suffix (was templated)
  /// to distinguish it from GetInterpolatedValue which returns Geant4 types and is
  /// the main interface to should have the clean name.
  virtual BDSFieldValue GetInterpolatedValueT(G4double x, G4double y, G4double z) const = 0;

  /// The field data.
  BDSArray3DCoords* array;
};

#endif
