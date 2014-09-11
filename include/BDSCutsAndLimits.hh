#ifndef __BDS_CUTS_AND_LIMITS_HH_
#define __BDS_CUTS_AND_LIMITS_HH_

#include "G4VPhysicsConstructor.hh"

class BDSCutsAndLimits: public G4VPhysicsConstructor{
public:
  BDSCutsAndLimits();
  ~BDSCutsAndLimits();
  void ConstructProcess();
  void ConstructParticle();
private:
  G4bool _wasActivated;
};
#endif
