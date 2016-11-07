#ifndef BDSPARAMETERISATIONPHYSICS_H
#define BDSPARAMETERISATIONPHYSICS_H

#include "G4VPhysicsConstructor.hh"
#include "G4FastSimulationManagerProcess.hh"

class BDSParameterisationPhysics: public G4VPhysicsConstructor
{
public:
  BDSParameterisationPhysics();
  virtual ~BDSParameterisationPhysics();
  void ConstructProcess();
  void ConstructParticle();
  
private:
  G4bool _wasActivated;
  G4FastSimulationManagerProcess* _fastSimulationManagerProcess;
};

#endif
