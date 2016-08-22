#ifndef BDSFIELDEGLOBAL_H
#define BDSFIELDEGLOBAL_H

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include "BDSAuxiliaryNavigator.hh"
#include "BDSFieldE.hh"

#include <utility>

/**
 * @brief A base class for electric fields in local to be used in global coordinates.
 * 
 * This base class provides the aggregative inheritance and utility functions
 * for magnetic fields in local coordinates to be used in global coordinates.
 * 
 * Constness is particularly important here as member functions are called
 * from inside GetField function which is const.
 *
 * This owns the field it wraps.
 * 
 * @author Laurie Nevay
 */

class BDSFieldEGlobal: public BDSFieldE, public BDSAuxiliaryNavigator
{
public:
  BDSFieldEGlobal(BDSFieldE* fieldIn,
		  G4bool     cacheTransforms);
  virtual ~BDSFieldEGlobal();

  /// Get the field - local coordinates. Apply the global to local transform,
  /// query the wrapped field object and transform this field to global
  /// coordinates before returning.
  G4ThreeVector GetField(const G4ThreeVector& position,
			 const G4double&      t) const;

  /// Necessary overload for Geant4
  virtual G4bool DoesFieldChangeEnergy() const {return true;}
  
private:
  /// Private default constructor to force use of supplied constructor
  BDSFieldEGlobal();

  /// The field on which this is based.
  BDSFieldE* field;
};

#endif
