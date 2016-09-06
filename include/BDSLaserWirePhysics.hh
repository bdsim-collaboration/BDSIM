#ifndef BDSLASERWIREPHYSICS_H
#define BDSLASERWIREPHYSICS_H

#include "G4VPhysicsConstructor.hh"

class BDSLaserWirePhysics: public G4VPhysicsConstructor
{
public:
  BDSLaserWirePhysics();
  virtual ~BDSLaserWirePhysics();

  virtual void ConstructProcess();
  virtual void ConstructParticle();

};

#endif
