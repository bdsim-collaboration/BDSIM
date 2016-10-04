#ifndef BDSFIELDEM_H
#define BDSFIELDEM_H

#include "globals.hh" // geant4 types / globals
#include "G4ElectroMagneticField.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include <utility>

/**
 * @brief Interface for BDSIM electro-magnetic fields that may or may not be local.
 * 
 * Defines abstract interface for all derived electro-magnetic fields. More common
 * to use G4ThreeVectors than arrays so provides common functionality
 * 
 * @author Laurie Nevay
 */

class BDSFieldEM: public G4ElectroMagneticField
{
public:
  BDSFieldEM();
  BDSFieldEM(G4Transform3D transformIn);
  virtual ~BDSFieldEM(){;}

  /// Get the field - local coordinates. First G4ThreeVector is B field, second is E Field
  /// x,y,z respectively.
  virtual std::pair<G4ThreeVector,G4ThreeVector> GetField(const G4ThreeVector& position,
							       const G4double& t) const = 0;
  
  /// Implement interface to this class's GetField to fulfill G4ElectroMagneticField
  /// inheritance and allow a BDSFieldEM instance to be passed around in the field
  /// factory even if it's not wrapped in a BDSFieldGlobal instance and is in fact
  /// in local coordinates.
  virtual void GetFieldValue(const G4double point[4],
			     G4double* field) const;

  /// Get the field value after applying transform for local offset.
  std::pair<G4ThreeVector,G4ThreeVector> GetFieldTransformed(const G4ThreeVector& position,
							     const G4double&      t) const;

  /// Required overload by Geant4.
  virtual G4bool DoesFieldChangeEnergy() const {return true;}
  
private:
  /// Transform to apply for the field relative to the local coordinates of the geometry.
  G4Transform3D transform;
};

#endif
