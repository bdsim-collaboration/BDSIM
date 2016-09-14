#ifndef BDSFIELDE_H
#define BDSFIELDE_H

#include "globals.hh" // geant4 types / globals
#include "G4ElectricField.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

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
  BDSFieldE(G4Transform3D transformIn);
  virtual ~BDSFieldE(){;}

  /// Get the field - local coordinates.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
				 const G4double&      t) const = 0;
  
  /// Implement interface to this class's GetField to fulfill G4ElectricField
  /// inheritance and allow a BDSFieldE instance to be passed around in the field
  /// factory even if it's not wrapped in a BDSFieldGlobal instance and is in fact
  /// in local coordinates.
  virtual void GetFieldValue(const G4double point[4],
			     G4double* field) const;

  /// Get the field value after applying transform for local offset.
  G4ThreeVector GetFieldTransformed(const G4ThreeVector& position,
				    const G4double&      t) const;

  /// Set the transform applied before evaluating the field. This can be used
  /// to account for any difference between the field coordinate system and
  /// the coordinate system of the geometry.  Ie an offset aperture.  This is
  /// a public interface to allow the transform to be set after construction so
  /// that derived classes don't need modified constructors.
  void SetTransform(const G4Transform3D& transformIn)
  {transform = transformIn.inverse();}

private:
  /// Transform to apply for the field relative to the local coordinates of the geometry.
  G4Transform3D transform;
};

#endif
