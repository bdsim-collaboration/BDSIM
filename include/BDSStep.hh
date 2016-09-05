#ifndef BDSSTEP_H
#define BDSSTEP_H

#include "G4ThreeVector.hh"

/**
 * @brief A simple class to represent the positions of a step.
 *
 * @author Laurie Nevay
 */

class BDSStep
{
public:
  BDSStep();
  BDSStep(G4ThreeVector preStepPointIn, G4ThreeVector postStepPointIn);
  ~BDSStep();

  /// @{ Accessor.
  inline G4ThreeVector PreStepPoint()  const {return preStepPoint;}
  inline G4ThreeVector PostStepPoint() const {return postStepPoint;}
  /// @}

  /// @{ Setter.
  inline void SetPreStepPoint(G4ThreeVector preIn)   {preStepPoint = preIn;}
  inline void SetPostStepPoint(G4ThreeVector postIn) {postStepPoint = postIn;}
  /// @}
  
private:
  G4ThreeVector preStepPoint;
  G4ThreeVector postStepPoint;
};

#endif
