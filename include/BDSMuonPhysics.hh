#ifndef BDSMUONPHYSICS_H
#define BDSMUONPHYSICS_H

#include "BDSDebug.hh"

#include "G4VPhysicsConstructor.hh"

/**
 * @brief High energy muon processes.
 *
 */

class BDSMuonPhysics: public G4VPhysicsConstructor
{
public:
  BDSMuonPhysics();
  virtual ~BDSMuonPhysics();
  virtual void ConstructProcess();
  virtual void ConstructParticle();
  
private:
  G4bool verbose;
#ifdef BDSDEBUG 
  bool debug = true;
#else 
  bool debug = false;
#endif
};
#endif
