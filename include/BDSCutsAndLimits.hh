#ifndef BDSCUTSANDLIMITS_H
#define BDSCUTSANDLIMITS_H

#include "G4VPhysicsConstructor.hh"

class G4StepLimiter;
class G4UserSpecialCuts;

/**
 * @brief Physics processes required for user tracking limits.
 */

class BDSCutsAndLimits: public G4VPhysicsConstructor
{
public:
  BDSCutsAndLimits();
  virtual ~BDSCutsAndLimits();
  virtual void ConstructProcess();
  virtual void ConstructParticle();
private:
  G4bool activated; ///< Cache of whether class is activated so it's no run twice.

  G4StepLimiter*     stepLimiter;  ///< Step limit process for MaxAllowedStep.
  G4UserSpecialCuts* specialCuts;  ///< Process for all other limits.
};
#endif
