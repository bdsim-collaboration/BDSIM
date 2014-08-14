#include "BDSCutsAndLimits.hh"
#include "BDSVProcess.hh"
#include "G4StepLimiter.hh"
#include "G4UserSpecialCuts.hh"
#include "G4ProcessManager.hh"

BDSCutsAndLimits::BDSCutsAndLimits():G4VPhysicsConstructor("BDSCutsAndLimits")
{;}

BDSCutsAndLimits::~BDSCutsAndLimits()
{;}

void BDSCutsAndLimits::ConstructParticle(){
  G4Gamma::Gamma();
  G4Electron::Electron();
  G4Positron::Positron();    
  G4Proton::Proton();
  return;
}

void BDSCutsAndLimits::ConstructProcess(){
  if(_wasActivated) return;
  _wasActivated=true;
  
  aParticleIterator->reset();
  while( (*aParticleIterator)() ){
    G4ParticleDefinition* particle = aParticleIterator->value();
    G4ProcessManager *pmanager = particle->GetProcessManager();
    if((particle->GetParticleName()=="gamma")||
       (particle->GetParticleName()=="e-")||
       (particle->GetParticleName()=="e+")||
       (particle->GetParticleName()=="proton")){
      particle->SetApplyCutsFlag(true);
    }
    pmanager->AddProcess(new G4StepLimiter,-1,-1,1);
#ifndef NOUSERSPECIALCUTS
    pmanager->AddDiscreteProcess(new G4UserSpecialCuts);
#endif
  }
  return;
}

