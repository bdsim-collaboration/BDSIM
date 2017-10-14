#ifndef BDSMUONPHYSICS_H
#define BDSMUONPHYSICS_H

#include "BDSSingleUse.hh"

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

class BDSPhysicsMuon: public G4VPhysicsConstructor, public BDSSingleUse
{
public:
  BDSPhysicsMuon();
  explicit BDSPhysicsMuon(G4bool emWillBeUsed);
  virtual ~BDSPhysicsMuon();
  /// Construct all leptions, photons (inc optical), and pion +- just in case.
  virtual void ConstructParticle();

  /// Construct and attach the processes to the relevant particles.
  virtual void ConstructProcess();

private:

  /// Whether em physics will be used, which provides processes that would be duplicate
  /// here.
  G4bool emWillBeUsed;
};
#endif
