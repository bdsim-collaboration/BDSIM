#ifndef BDSCUTSANDLIMITS_H
#define BDSCUTSANDLIMITS_H

#include "G4VPhysicsConstructor.hh"

class G4StepLimiter;
class G4UserSpecialCuts;

class BDSCutsAndLimits: public G4VPhysicsConstructor
{
public:
  BDSCutsAndLimits();
  virtual ~BDSCutsAndLimits();
  void ConstructProcess();
  void ConstructParticle();
private:
  G4bool _wasActivated;
  G4StepLimiter* stepLimiter;
  G4UserSpecialCuts* specialCuts;
};
#endif
