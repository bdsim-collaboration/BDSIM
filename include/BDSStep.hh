#ifndef BDSSTEP_H
#define BDSSTEP_H

#include "G4ThreeVector.hh"

class G4VPhysicalVolume;

/**
 * @brief A simple class to represent the positions of a step.
 *
 * @author Laurie Nevay
 */

class BDSStep
{
public:
  BDSStep();
  BDSStep(G4ThreeVector preStepPointIn,
	  G4ThreeVector postStepPointIn,
	  G4VPhysicalVolume* volumeForTransformIn = nullptr);
  ~BDSStep();

  /// @{ Accessor.
  inline G4ThreeVector PreStepPoint()  const {return preStepPoint;}
  inline G4ThreeVector PostStepPoint() const {return postStepPoint;}
  inline G4VPhysicalVolume* VolumeForTransform() const {return volumeForTransform;}
  /// @}

  /// @{ Setter.
  inline void SetPreStepPoint(G4ThreeVector preIn)   {preStepPoint = preIn;}
  inline void SetPostStepPoint(G4ThreeVector postIn) {postStepPoint = postIn;}
  inline void SetVolumeForTransform(G4VPhysicalVolume* volIn) {volumeForTransform = volIn;}
  /// @}
  
private:
  G4ThreeVector preStepPoint;
  G4ThreeVector postStepPoint;
  G4VPhysicalVolume* volumeForTransform; ///< The volume that was used for the transform.
};

#endif
