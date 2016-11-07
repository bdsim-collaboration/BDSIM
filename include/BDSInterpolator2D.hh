#ifndef BDSINTERPOLATOR2D_H
#define BDSINTERPOLATOR2D_H

#include "BDSFieldValue.hh"

#include "G4Types.hh"
#include "G4ThreeVector.hh"

class BDSArray2DCoords;

/**
 * @brief Interface for all 2D interpolators.
 *
 * Does not own data - so multiple interpolators could be used on same data.
 *
 * @author Laurie Nevay
 */

class BDSInterpolator2D
{
public:
  BDSInterpolator2D(BDSArray2DCoords* arrayIn);
  virtual ~BDSInterpolator2D(){;}

  /// Public interface to a 2D interpolator. Returns Geant4 type as that's what will be needed.
  G4ThreeVector GetInterpolatedValue(G4double x, G4double y) const;

  /// Alternative public interface to allow use of a 3-vector position (ignores z).
  /// Returns Geant4 type as that's what will be needed.
  G4ThreeVector GetInterpolatedValue(G4ThreeVector position) const;

  inline const BDSArray2DCoords* Array() const {return array;}

protected:
  /// Each derived class should implement this function. Note T suffix (was templated)
  /// to distinguish it from GetInterpolatedValue which returns Geant4 types and is
  /// the main interface to should have the clean name.
  virtual BDSFieldValue GetInterpolatedValueT(G4double x, G4double y) const = 0;
  
  /// The field data.
  BDSArray2DCoords* array;
};

#endif
