#ifndef BDSSYNCHRADPHYSICS_H
#define BDSSYNCHRADPHYSICS_H

#include "globals.hh" // geant4 types / globals
#include "G4VPhysicsConstructor.hh"

/**
 * @brief A physics constructor that only constructs Synchrotron Radiation.
 */

class BDSSynchRadPhysics: public G4VPhysicsConstructor
{
public:
  BDSSynchRadPhysics();
  virtual ~BDSSynchRadPhysics();

  /// Construct all leptons and the photon.
  virtual void ConstructParticle();
  
  /// Construct and attach G4SynchrotronRadiation to all charged particles.
  virtual void ConstructProcess();
};

#endif
