#ifndef BDSFIELDMAGGLOBAL_H
#define BDSFIELDMAGGLOBAL_H

#include "globals.hh" // geant4 types / globals
#include "G4MagneticField.hh"
#include "G4ThreeVector.hh"

#include "BDSAuxiliaryNavigator.hh"

class BDSField;

/**
 * @brief A base class for magnetic fields in local coordinates.
 * 
 * This base class provides the aggregative inheritance and utility functions
 * for magnetic fields in local coordinates.
 * 
 * Constness is particularly important here as member functions are called
 * from inside G4MagneticField::GetField function which is const.
 * 
 * @author Laurie Nevay
 */

class BDSFieldMagGlobal: public G4MagneticField, public BDSAuxiliaryNavigator
{
public:
  BDSFieldMagGlobal(BDSField* fieldObjectIn, G4ThreeVector localOffsetIn = G4ThreeVector(0,0,0));
  ~BDSFieldMagGlobal(){;}

  /// Accessor that overloads Geant4 pure virtual method. Gets the coordinates at global
  /// position "point[4]" by converting to local coordinates and adding the local offset
  /// this field was constructed with, then converting back to global coordinates and writing
  /// the value to *field.
  virtual void GetFieldValue(const G4double point[4],
			     G4double* field) const;
  
private:
  /// Private default constructor to force use of supplied constructor
  BDSFieldMagGlobal();

  BDSField* fieldObject;

  /// Offset between that is applied - use in case field is intended to be offset
  /// from local coordinates.
  G4ThreeVector localOffset;
};

#endif
