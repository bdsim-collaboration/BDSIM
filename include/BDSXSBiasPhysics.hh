#ifndef __BDS_XS_BIAS_PHYSICS_HH_
#define __BDS_XS_BIAS_PHYSICS_HH_

#include "G4VPhysicsConstructor.hh"
#include "BDSXSBias.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "BDSVProcess.hh"
#include "G4GammaConversionToMuons.hh"

class BDSXSBiasPhysics: public G4VPhysicsConstructor{
public:
  BDSXSBiasPhysics(G4String name);
  ~BDSXSBiasPhysics();
  virtual void ConstructProcess();
  virtual void ConstructParticle();

protected:
  G4bool _wasActivated;
  void ReplaceDiscreteProcess(G4ProcessManager* pmanager, G4VProcess* proc1, G4VProcess* proc2);
  
private:
  BDSXSBiasPhysics();

};
#endif
