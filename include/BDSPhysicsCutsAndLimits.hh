#ifndef BDSCUTSANDLIMITS_H
#define BDSCUTSANDLIMITS_H

#include "BDSSingleUse.hh"

#include "G4VPhysicsConstructor.hh"

class G4StepLimiter;
class G4UserSpecialCuts;

/**
 * @brief Physics processes required for user tracking limits.
 */

class BDSPhysicsCutsAndLimits: public G4VPhysicsConstructor, public BDSSingleUse
{
public:
  BDSPhysicsCutsAndLimits();
  virtual ~BDSPhysicsCutsAndLimits();

  /// Construct gamma, e+- and proton - the minimum this sets limits for.
  virtual void ConstructParticle();

  /// Construct and attach step limiter and cuts processes.
  virtual void ConstructProcess();

private:
  G4StepLimiter*     stepLimiter;  ///< Step limit process for MaxAllowedStep.
  G4UserSpecialCuts* specialCuts;  ///< Process for all other limits.
};
#endif
