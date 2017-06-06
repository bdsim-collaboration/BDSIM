#ifndef BDSPHYSICSCHERENKOV_H
#define BDSPHYSICSCHERENKOV_H

#include "BDSSingleUse.hh"

#include "globals.hh"
#include "G4VPhysicsConstructor.hh"

/**
 * @brief Cherenkov physics process constructor.
 *
 * @author Laurie Nevay
 */

class BDSPhysicsCherenkov: public G4VPhysicsConstructor, public BDSSingleUse
{
public:
  BDSPhysicsCherenkov(G4double maxPhotonsPerStepIn,
		      G4double maxBetaChangePerStepIn);
  virtual ~BDSPhysicsCherenkov();

  virtual void ConstructParticle();
  virtual void ConstructProcess();

private:

  BDSPhysicsCherenkov() = delete;
  
  /// @{ Parameter from global constants to control cherenkov physics.
  G4double maxPhotonsPerStep;
  G4double maxBetaChangePerStep;
  /// @}
};

#endif
