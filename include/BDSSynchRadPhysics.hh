#ifndef __BDS_SYNCH_RAD_PHYSICS_HH_
#define __BDS_SYNCH_RAD_PHYSICS_HH_

#include "G4VPhysicsConstructor.hh"
#include "G4SynchrotronRadiation.hh"
#include "BDSSynchrotronRadiation.hh"
#include "BDSContinuousSR.hh"

class BDSSynchRadPhysics: public G4VPhysicsConstructor{
public:
  BDSSynchRadPhysics();
  ~BDSSynchRadPhysics();
  void ConstructProcess();
  void ConstructParticle();
  
private:
  BDSSynchrotronRadiation* _srProcess;
  G4SynchrotronRadiation* _srProcessG4;
  BDSContinuousSR* _contSR;
  G4bool _wasActivated;
};

#endif
