#include "BDSSynchRadPhysics.hh"
#include "G4Gamma.hh"
#include "G4ProcessManager.hh"
#include "G4Version.hh"

BDSSynchRadPhysics::BDSSynchRadPhysics():G4VPhysicsConstructor("BDSSynchRadPhysics"),
					 _srProcess(NULL),_contSR(NULL),_wasActivated(false)
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


#if G4VERSION_NUMBER < 1000
  theParticleTable    = G4ParticleTable::GetParticleTable();
  theParticleIterator = theParticleTable->GetIterator();
  G4ParticleTable::G4PTblDicIterator* aParticleIterator = theParticleIterator;
#endif

  aParticleIterator->reset();
  while( (*aParticleIterator)() ){
    G4ParticleDefinition* particle = aParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    if (particleName == "e-") {
      pmanager->AddProcess(_srProcess);
      pmanager->SetProcessOrderingToLast(_srProcess,idxPostStep);
      G4int idx = pmanager->AddProcess(_contSR);
      pmanager->SetProcessOrderingToLast(_contSR,idxPostStep);
      pmanager->SetProcessActivation(idx, false);
    }
    if (particleName == "e+") {
      pmanager->AddProcess(_srProcess);
      pmanager->SetProcessOrderingToLast(_srProcess,idxPostStep);
      G4int idx = pmanager->AddProcess(_contSR);
      pmanager->SetProcessOrderingToLast(_contSR,idxPostStep);
      pmanager->SetProcessActivation(idx, false);
    }
  }
  return;
}


