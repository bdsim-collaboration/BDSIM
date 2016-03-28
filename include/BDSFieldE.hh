#ifndef BDSFIELDE_H
#define BDSFIELDE_H

#include "globals.hh" // geant4 types / globals
#include "G4ElectricField.hh"
#include "G4ThreeVector.hh"

/**
 * @brief Interface for BDSIM electric fields that may or may not be local.
 * 
 * Defines abstract interface for all derived electric fields. More common
 * to use G4ThreeVectors than arrays so provides common functionality
 * 
 * @author Laurie Nevay
 */

class BDSFieldE: public G4ElectricField
{
public:
  BDSFieldE();
  virtual ~BDSFieldE(){;}

  /// Get the field - local coordinates.
  virtual G4ThreeVector GetFieldValue(const G4ThreeVector& position) const = 0;
  
  /// Implement interface to this class's GetField to fulfill G4ElectricField
  /// inheritance and allow a BDSFieldE instance to be passed around in the field
  /// factory even if it's not wrapped in a BDSFieldGlobal instance and is in fact
  /// in local coordinates.
  virtual void GetFieldValue(const G4double point[4],
			     G4double* field) const;
  
};

#endif
