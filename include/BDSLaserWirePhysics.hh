#ifndef BDSLASERWIREPHYSICS_H
#define BDSLASERWIREPHYSICS_H

#include "G4VPhysicsConstructor.hh"

/**
 * @brief Constructor for BDSLaserCompton process.
 */

class BDSLaserWirePhysics: public G4VPhysicsConstructor
{
public:
  BDSLaserWirePhysics();
  virtual ~BDSLaserWirePhysics();

  virtual void ConstructProcess();
  virtual void ConstructParticle();
};

#endif
