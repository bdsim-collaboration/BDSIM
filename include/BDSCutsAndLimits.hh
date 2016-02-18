#ifndef BDSCUTSANDLIMITS_H
#define BDSCUTSANDLIMITS_H

#include "G4VPhysicsConstructor.hh"

class BDSCutsAndLimits: public G4VPhysicsConstructor
{
public:
  BDSCutsAndLimits();
  virtual ~BDSCutsAndLimits();
  void ConstructProcess();
  void ConstructParticle();
private:
  G4bool _wasActivated;
};
#endif
