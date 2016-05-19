#ifndef BDSAUXILIARYNAVIGATOR_H
#define BDSAUXILIARYNAVIGATOR_H

#include "G4Navigator.hh"

#include <utility>

class G4VPhysicalVolume;

/**
 * @brief Extra G4Navigator object common to all steppers to get coordinate
 * transforms.
 * 
 * All BDSIM integrators and magnetic fields require the ability 
 * to convert from global to local coordinates. The prescribed method 
 * to do this is by using a G4Navigator instance. There is of course the 
 * main navigator for tracking, but requesting information on a global
 * point changes the state of the navigator - ie the particle being 
 * tracked by the navigator is now that point. This is VERY dangerous.
 * 
 * This class provides a static auxiliary navigator that each derived 
 * class can use. Making the auxilliary navigator static is not done 
 * to reduce memory usage but because navigating from an unknown place 
 * to anywhere in the geometry is
 * much more costly than a relative move in the geometry. If we only
 * use one auxiliary navigator, it will always be relatively close
 * in the geometry even if a different stepper has been used and 
 * therefore more efficient. This is important as Geant4 may use the
 * steppers at least two or three times to estimate the best next step
 * and the stepper itself may make three steps (full, and two half) to
 * estimate the error in the integration.
 *
 * Inheritance of this class assumes that the field or integrator is only 
 * every attached to ONE single volume.  A single volume has a unique transform
 * between local and global coordinates that needn't be searched for (through Geant4) each
 * and every time the field is queried. This class stores that transform after the first 
 * look up.
 * 
 * @author Laurie Nevay
 */

class BDSAuxiliaryNavigator
{
public:
  BDSAuxiliaryNavigator();
  ~BDSAuxiliaryNavigator();

  static void AttachWorldVolumeToNavigator(G4VPhysicalVolume* worldPV);

  /// Locate the supplied point the in the geometry and get and store
  /// the transform to that volume in the member variable. This function
  /// has to be const as it's called the first time in GetField which is
  /// a pure virtual const function from G4MagneticField that we have to
  /// implement and have to keep const. This function doesn't change the
  /// const pointer but does change the contents of what it points to.
  void InitialiseTransform(const G4ThreeVector& globalPosition) const;

  /// Calculate the local coordinates of a global point.
  G4ThreeVector ConvertToLocal(const G4double globalPoint[3]) const;

  /// Vector version
  G4ThreeVector ConvertToLocal(const G4ThreeVector& globalPoint) const;

  /// Calculate the local version of a global vector (axis). This is done
  /// w.r.t. a point - ie, the point is used to initialise the transform
  /// if not done already.
  G4ThreeVector ConvertAxisToLocal(const G4double globalPoint[3],
				   const G4double globalAxis[3]) const;

  /// Vector version
  G4ThreeVector ConvertAxisToLocal(const G4ThreeVector& globalPoint,
				   const G4ThreeVector& globalAxis) const;

  /// Convert a vector (axis) from local to global coordinates. NOTE this
  /// function must only be used once the instance of this class has been initialised,
  /// setting up the transforms.  It is up to the developer to ensure this,
  /// otherwise you'll find a bad access.
  G4ThreeVector ConvertAxisToGlobal(const G4ThreeVector& localAxis) const;

  /// Convert a vector (axis) from local to global coordinates. Note this function
  /// must only be used once the instance of this class has been initialised, setting
  /// up the transforms. It is up to the developer to ensure this. This utility function
  /// operates on two threevectors in a pair.
  std::pair<G4ThreeVector, G4ThreeVector> ConvertAxisToGlobal(const std::pair<G4ThreeVector, G4ThreeVector>& localAxis) const;

  /// Convert a position in local coordinates to global coordinates. NOTE a
  /// similar caution to ConvertAxisToGlobal applies.
  G4ThreeVector ConvertToGlobal(const G4ThreeVector& localPosition) const;

  /// Convert a vector (axis) from local to global coordinates. This uses a global
  /// position to ensure the transform is initialised.
  G4ThreeVector ConvertAxisToGlobal(const G4ThreeVector& globalPosition,
				    const G4ThreeVector& localAxis) const;

  /// Convert a position in local coordinates to global coordinates. This uses a
  /// global position to ensure the transform is initialised.
  G4ThreeVector ConvertToGlobal(const G4ThreeVector& globalPosition,
				const G4ThreeVector& localPosition) const;

protected:
  /// Whether this instance has been intialised. If not store the transform
  /// this field requires and record as initialised so not repeated.
  mutable G4bool initialised;

  /// Transform for this particular field depending on which part of the geometry
  /// it's attached to. This has to be a pointer as the Geant4 GetField function is
  /// const - which means it can't change the member variable. Using a pointer, we *can*
  /// change the contents of the pointer, but *not* the pointer itself.
  G4AffineTransform* const globalToLocal;
  G4AffineTransform* const localToGlobal;
  
  static G4Navigator* auxNavigator;
};


#endif
