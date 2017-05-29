#ifndef BDSMUONPHYSICS_H
#define BDSMUONPHYSICS_H

#include "G4VPhysicsConstructor.hh"

/**
 * @brief High energy muon processes.
 * 
 * Constructs:
 * Gamma to Muon pair.
 * e+ annihilation to Muon pair
 * e+ annihilation to hadrons.
 * Muon multiple scattering.
 * Muon ionisation.
 * Muon bremsstrahlung.
 * Muon pair production from Muons.
 *
 * Uses the G4PhysicsListHelper to set and order processes.
 *
 */

class BDSMuonPhysics: public G4VPhysicsConstructor
{
public:
  BDSMuonPhysics();
  virtual ~BDSMuonPhysics();
  /// Construct all leptions, photons (inc optical), and pion +- just in case.
  virtual void ConstructParticle();

  /// Construct and attach the processes to the relevant particles.
  virtual void ConstructProcess();
};
#endif
