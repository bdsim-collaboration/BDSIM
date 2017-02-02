#ifndef BDSPARAMETERISATIONPHYSICS_H
#define BDSPARAMETERISATIONPHYSICS_H

#include "G4VPhysicsConstructor.hh"
#include "G4FastSimulationManagerProcess.hh"

/** 
 * @brief G4 fast simulation manager constructor to allowed parameterised showers.
 */

class BDSParameterisationPhysics: public G4VPhysicsConstructor
{
public:
  BDSParameterisationPhysics();
  virtual ~BDSParameterisationPhysics(){;}
  void ConstructProcess();
  void ConstructParticle();
  
private:
  G4bool activated;
  G4FastSimulationManagerProcess* fastSimulationManagerProcess;
};

#endif
