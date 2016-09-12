#ifndef BDSFIELDEMGLOBAL_H
#define BDSFIELDEMGLOBAL_H

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include "BDSAuxiliaryNavigator.hh"
#include "BDSFieldEM.hh"

#include <utility>

/**
 * @brief A base class for electro-magnetic fields in local to be used in global coordinates.
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

class BDSFieldEMGlobal: public BDSFieldEM, public BDSAuxiliaryNavigator
{
public:
  BDSFieldEMGlobal(BDSFieldEM* fieldIn);
  virtual ~BDSFieldEMGlobal();

  /// Get the field - local coordinates. First G4ThreeVector is B field, second is E Field
  /// x,y,z respectively. Apply the global to local transform, query the wrapped field object
  /// and transform this field to global coordinates before returning.
  virtual std::pair<G4ThreeVector,G4ThreeVector> GetField(const G4ThreeVector& position,
                                                          const G4double&      t) const;

  /// Necessary overload for Geant4
  virtual G4bool DoesFieldChangeEnergy() const {return true;}
  
private:
  /// Private default constructor to force use of supplied constructor
  BDSFieldEMGlobal();

  /// The field on which this is based.
  BDSFieldEM* field;
};

#endif
