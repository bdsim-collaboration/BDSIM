#include "BDSSynchRadPhysics.hh"
#include "G4Gamma.hh"
#include "G4ProcessManager.hh"

BDSSynchRadPhysics::BDSSynchRadPhysics():G4VPhysicsConstructor("BDSSynchRadPhysics"),
					 _srProcess(nullptr),_srProcessG4(nullptr),_contSR(nullptr),_wasActivated(false)
{;}

BDSSynchRadPhysics::~BDSSynchRadPhysics()
{;}

void BDSSynchRadPhysics::ConstructParticle(){
  G4Gamma::Gamma();
  G4Electron::Electron();
  G4Positron::Positron();    
  return;
}

void BDSSynchRadPhysics::ConstructProcess(){
  if(_wasActivated) return;
  _wasActivated = true;
  _srProcess    = new BDSSynchrotronRadiation();
  _srProcessG4  = new G4SynchrotronRadiation();
  _contSR       = new BDSContinuousSR();

  aParticleIterator->reset();
  while( (*aParticleIterator)() ){
    G4ParticleDefinition* particle = aParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    if (particleName == "e-") {
      pmanager->AddProcess(_srProcess);
      pmanager->SetProcessOrderingToLast(_srProcess,idxPostStep);
      //G4int idx = pmanager->AddProcess(_contSR);
      //pmanager->SetProcessOrderingToLast(_contSR,idxPostStep);
      //pmanager->SetProcessActivation(idx, false);
    }
    if (particleName == "e+") {
      pmanager->AddProcess(_srProcess);
      pmanager->SetProcessOrderingToLast(_srProcess,idxPostStep);
      //G4int idx = pmanager->AddProcess(_contSR);
      //pmanager->SetProcessOrderingToLast(_contSR,idxPostStep);
      //pmanager->SetProcessActivation(idx, false);
    }
  }
  return;
}


