#ifndef __BDS_PARAMETERISATION_PHYSICS_HH_
#define __BDS_PARAMETERISATION_PHYSICS_HH_

#include "G4VPhysicsConstructor.hh"
#include "G4FastSimulationManagerProcess.hh"

class BDSParameterisationPhysics: public G4VPhysicsConstructor {
public:
  BDSParameterisationPhysics();
  ~BDSParameterisationPhysics();
  void ConstructProcess();
  void ConstructParticle();
  
private:
  G4bool _wasActivated;
  G4FastSimulationManagerProcess* _fastSimulationManagerProcess;
};

#endif
