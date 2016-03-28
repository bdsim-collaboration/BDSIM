#ifndef BDSFIELDMAGGLOBAL_H
#define BDSFIELDMAGGLOBAL_H

#include "globals.hh" // geant4 types / globals
#include "G4MagneticField.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include "BDSAuxiliaryNavigator.hh"
#include "BDSFieldMag.hh"

/**
 * @brief A base class for magnetic fields in local to be used in global coordinates.
 * 
 * This base class provides the aggregative inheritance and utility functions
 * for magnetic fields in local coordinates to be used in global coordinates.
 * 
 * Constness is particularly important here as member functions are called
 * from inside G4MagneticField::GetField function which is const.
 *
 * This owns the field it wraps.
 * 
 * @author Laurie Nevay
 */

class BDSFieldMagGlobal: public BDSFieldMag, public BDSAuxiliaryNavigator
{
public:
  BDSFieldMagGlobal(BDSFieldMag* fieldIn);
  virtual ~BDSFieldMagGlobal();

  /// Apply the global to local transform, query the wrapped field object
  /// and transform this field to global coordinates before returning.
  virtual G4ThreeVector GetField(const G4ThreeVector &position) const;
  
private:
  /// Private default constructor to force use of supplied constructor
  BDSFieldMagGlobal();

  /// The field on which this is based.
  BDSFieldMag* field;
};

#endif
