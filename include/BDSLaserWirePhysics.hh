#ifndef BDSLASERWIREPHYSICS_H
#define BDSLASERWIREPHYSICS_H

#include "G4VPhysicsConstructor.hh"

/**
 * @brief Constructor for BDSLaserCompton process.
 * 
 * This class doesn't use the G4PhyiscsListHelper as it
 * doesn't know about our physics process.
 */

class BDSLaserWirePhysics: public G4VPhysicsConstructor
{
public:
  BDSLaserWirePhysics();
  virtual ~BDSLaserWirePhysics();

  /// Construct electrons and positrons and the photon.
  virtual void ConstructParticle();

  /// Construct and attache BDSIM laserwire process.
  virtual void ConstructProcess();
};

#endif
