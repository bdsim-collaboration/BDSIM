#ifndef BDSAUXILIARYNAVIGATOR_H
#define BDSAUXILIARYNAVIGATOR_H

#include "G4Navigator.hh"

/**
 * @brief All BDSIM steppers and magnetic fields require the ability 
 * to convert from global to local coordinates. The prescribed method 
 * to do this is by using a G4Navigator instance. There is of course the 
 * main navigator for tracking, but requesting information on a global
 * point chanages the state of the navigator - ie the particle as 
 * tracked by the navigator is now that point. This is VERY dangerous.
 * 
 * This class provides a static auxiliary navigator that each derived 
 * class can use. This (being static) is not done to reduce memory usage but because
 * a navigating from an unknown place to anywhere in the geometry is
 * much more costly than a relative move in the geometry. If we only
 * use one auxiliary navigator, it will always be relatively close
 * in the geometry even if a different stepper has been used and 
 * therefore more efficient. This is important as Geant4 may use the
 * steppers at least two or three times to estimate the best next step
 * and the stepper itself may make three steps (full, and two half) to
 * estimate the error in the integration.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class G4VPhysicalVolume;

class BDSAuxiliaryNavigator
{
public:
  BDSAuxiliaryNavigator();

  static void AttachWorldVolumeToNavigator(G4VPhysicalVolume* worldPV);

protected:
  static G4Navigator* auxNavigator;
};


#endif
