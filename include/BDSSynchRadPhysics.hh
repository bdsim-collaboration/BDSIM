#ifndef BDSSYNCHRADPHYSICS_H
#define BDSSYNCHRADPHYSICS_H

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
