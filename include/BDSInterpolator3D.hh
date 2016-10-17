#ifndef BDSINTERPOLATOR3D_H
#define BDSINTERPOLATOR3D_H

#include "BDSFieldValue.hh"

#include "G4Types.hh"
#include "G4ThreeVector.hh"

/**
 * @brief Interface for all 3D interpolators.
 *
 * Derived class should not own data - so multiple interpolators could be used on same data.
 *
 * @author Laurie Nevay
 */

class BDSInterpolator3D
{
public:
  BDSInterpolator3D(){;}
  virtual ~BDSInterpolator3D(){;}

  /// Public interface to a 3D interpolator. Returns Geant4 type as that's what will be needed.
  G4ThreeVector GetInterpolatedValue(G4double x, G4double y, G4double z) const
  {
    BDSFieldValue r = GetInterpolatedValueT(x,y,z);
    return G4ThreeVector(r.x(), r.y(), r.z());
  }

  /// Alternative public interface to allow use of a 3-vector position (ignores z).
  /// Returns Geant4 type as that's what will be needed.
  G4ThreeVector GetInterpolatedValue(G4ThreeVector position) const
  {
    BDSFieldValue r = GetInterpolatedValueT(position.x(), position.y(), position.z());
    return G4ThreeVector(r.x(), r.y(), r.z());
  }
  
protected:
  /// Each derived class should implement this function. Note T suffix (was templated)
  /// to distinguish it from GetInterpolatedValue which returns Geant4 types and is
  /// the main interface to should have the clean name.
  virtual BDSFieldValue GetInterpolatedValueT(G4double x, G4double y, G4double z) const = 0;
};

#endif
