#ifndef BDSFIELDMAGBASE_H
#define BDSFIELDMAGBASE_H

#include "globals.hh" // geant4 types / globals
#include "G4MagneticField.hh"
#include "G4ThreeVector.hh"

#include "BDSAuxiliaryNavigator.hh"

/**
 * @brief A base class for magnetic fields in local coordinates.
 * 
 * This base class provides the aggregative inheritance and utility functions
 * for magnetic fields in local coordinates. Common tasks are provided in two
 * utility functions.
 * 
 * Constness is particularly important here as these utility functions are called
 * from inside G4MagneticField::GetField function which is const.
 * 
 * @author Laurie Nevay
 */

class BDSFieldMagBase: public G4MagneticField, public BDSAuxiliaryNavigator
{
public:
  BDSFieldMagBase();
  ~BDSFieldMagBase(){;}

  /// Calculate the local coordinates of a global point for this field. Changes
  /// protected member localPosition.  This has to be const because it's called
  /// inside the const Geant4 pure virtual function GetFieldValue.
  G4ThreeVector ConvertToLocal(const G4double (&point)[4]) const;

  /// Convert the supplied local magnetic field vector to global coordinates
  /// and write it to the supplied pointer address - 6 values in total (3 B and
  /// 3 E field).
  void OutputGlobalField(const G4ThreeVector& localField, G4double* field) const;
};

#endif
