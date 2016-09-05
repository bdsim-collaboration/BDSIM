#ifndef BDSAUXILIARYNAVIGATOR_H
#define BDSAUXILIARYNAVIGATOR_H

#include "globals.hh" // geant4 types / globals
#include "G4Navigator.hh"
#include "G4ThreeVector.hh"

class G4AffineTransform;
class G4Step;
class G4VPhysicalVolume;

/**
 * @brief Extra G4Navigator object common to all steppers to get coordinate
 * transforms.
 * 
 * All BDSIM steppers and magnetic fields require the ability 
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
 * This class in fact provides access two static navigators. The intended
 * use is one for the real world and one for the read out geometry / world
 * for curvilinear coordinates.  All functions have an optional last argument
 * to select which navigator is required - the default is the curvilinear one.
 * 
 * @author Laurie Nevay
 */

class BDSAuxiliaryNavigator
{
public:
  BDSAuxiliaryNavigator();
  BDSAuxiliaryNavigator(G4bool useCachingIn);
  ~BDSAuxiliaryNavigator();

  /// Setup the navigator w.r.t. to a world volume - typically real world.
  static void AttachWorldVolumeToNavigator(G4VPhysicalVolume* worldPV)
  {auxNavigator->SetWorldVolume(worldPV);}

  /// Setup the navigator w.r.t. to the read out world / geometry to provide
  /// curvilinear coordinates.
  static void AttachWorldVolumeToNavigatorCL(G4VPhysicalVolume* curvilinearWorldPV)
  {auxNavigatorCL->SetWorldVolume(curvilinearWorldPV);}

  /// A wrapper for the underlying static navigator instance located within this class.
  G4VPhysicalVolume* LocateGlobalPointAndSetup(const G4ThreeVector& point,
					       const G4ThreeVector* direction = nullptr,
					       const G4bool pRelativeSearch   = true,
					       const G4bool ignoreDirection   = true,
					       G4bool useCurvilinear          = true);

  /// A safe way to locate and setup a point inside a volume. Very const
  /// access to step.
  G4VPhysicalVolume* LocateGlobalPointAndSetup(G4Step const* const step,
					       G4bool useCurvilinear = true) const;

  /// Locate the supplied point the in the geometry and get and store
  /// the transform to that volume in the member variable. This function
  /// has to be const as it's called the first time in GetField which is
  /// a pure virtual const function from G4MagneticField that we have to
  /// implement and have to keep const. This function doesn't change the
  /// const pointer but does change the contents of what it points to.
  void InitialiseTransform(const G4ThreeVector& globalPosition) const;

  /// Calculate the local coordinates of a global point.
  G4ThreeVector ConvertToLocal(const G4double globalPoint[3],
			       const G4bool&  useCurvilinear = true) const;

  /// Vector version
  G4ThreeVector ConvertToLocal(const G4ThreeVector& globalPosition,
			       const G4bool&        useCurvilinear = true) const;

  /// Calculate the local version of a global vector (axis). This is done
  /// w.r.t. a point - ie, the point is used to initialise the transform
  /// if not done already.
  G4ThreeVector ConvertAxisToLocal(const G4double globalPoint[3],
				   const G4double globalAxis[3],
				   const G4bool&  useCurvilinear = true) const;

  /// Vector version
  G4ThreeVector ConvertAxisToLocal(const G4ThreeVector& globalPoint,
				   const G4ThreeVector& globalAxis,
				   const G4bool&        useCurvilinear = true) const;

  /// Convert a vector (axis) from local to global coordinates. NOTE this
  /// function must only be used once the instance of this class has been initialised,
  /// setting up the transforms.  It is up to the developer to ensure this,
  /// otherwise you'll find a bad access.
  G4ThreeVector ConvertAxisToGlobal(const G4ThreeVector& localAxis,
				    const G4bool&        useCurvilinear = true) const;

  /// Convert a vector (axis) from local to global coordinates. Note this function
  /// must only be used once the instance of this class has been initialised, setting
  /// up the transforms. It is up to the developer to ensure this. This utility function
  /// operates on two threevectors in a pair.
  std::pair<G4ThreeVector, G4ThreeVector> ConvertAxisToGlobal(const std::pair<G4ThreeVector, G4ThreeVector>& localAxis,
							      const G4bool&        useCurvilinear = true) const;

  /// Convert a position in local coordinates to global coordinates. NOTE a
  /// similar caution to ConvertAxisToGlobal applies.
  G4ThreeVector ConvertToGlobal(const G4ThreeVector& localPosition,
				const G4bool&        useCurvilinear = true) const;

  /// Convert a vector (axis) from local to global coordinates. This uses a global
  /// position to ensure the transform is initialised.
  G4ThreeVector ConvertAxisToGlobal(const G4ThreeVector& globalPosition,
				    const G4ThreeVector& localAxis,
				    const G4bool&        useCurvilinear = true) const;

  /// Convert a position in local coordinates to global coordinates. This uses a
  /// global position to ensure the transform is initialised.
  G4ThreeVector ConvertToGlobal(const G4ThreeVector& globalPosition,
				const G4ThreeVector& localPosition,
				const G4bool&        useCurvilinear = true) const;
  
protected:
  mutable G4bool initialised;

  G4AffineTransform* const globalToLocal;
  G4AffineTransform* const localToGlobal;
  G4AffineTransform* const globalToLocalCL;
  G4AffineTransform* const localToGlobalCL;
  
  /// Navgiator object for safe navigation in the real (mass) world without affecting
  /// tracking of the particle.
  static G4Navigator* auxNavigator;

  /// Navigator object for safe navigation in the read out world providing curvilinear
  /// coordinates for various applications. Since this is not a 'parallel' world but
  /// a 'read out geometry' in Geant4 terms, this is inherintly safe and won't affect
  /// the tracking of the particle in question.
  static G4Navigator* auxNavigatorCL;

private:
  /// Utility function to select appropriate navigator
  G4Navigator* Navigator(G4bool curvilinear) const;

  /// @{ Utility function to select appropriate transfrom.
  G4AffineTransform* GlobalToLocal(G4bool curvilinear) const;
  G4AffineTransform* LocalToGlobal(G4bool curvilinear) const;
  /// @}
  
  /// Whether caching of transforms should be used. If used, use of a BDSAuxiliaryNavigator
  /// instance is more efficient but that instance can only be used for one volume (asuuming
  /// the masss world is being used for the navigator). Whereas, if the caching is not used,
  /// a single instance of this class can be used to locate points anywhere in the setup.
  G4bool useCaching;
  
  /// Counter to keep track of when the last instance of the class is deleted and
  /// therefore when the navigators can be safely deleted without affecting
  static G4int numberOfInstances;
};


#endif
