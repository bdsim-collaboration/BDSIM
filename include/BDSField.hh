#ifndef BDSFIELD_H
#define BDSFIELD_H

#include "G4ThreeVector.hh"

/**
 * @brief Interface for BDSIM fields that may or may not be local.
 * 
 * Defines abstract interface for all derived fields. This allows these
 * simple field objects to be wrapped in Geant4 compatible fields with or
 * without conversion from global to local coordinates.
 * 
 * @author Laurie Nevay
 */

class BDSField
{
public:
  BDSField();
  virtual ~BDSField(){;}

  /// Get the field - local coordinates.
  virtual G4ThreeVector GetFieldValue(const G4ThreeVector& position) const = 0;

  virtual G4String Name() const = 0;
  
};

#endif
