#include "BDSParameterisationPhysics.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4FastSimulationManagerProcess.hh"
#include "G4Positron.hh"
#include "G4ProcessManager.hh"
#include "G4Version.hh"

BDSParameterisationPhysics::BDSParameterisationPhysics():
  G4VPhysicsConstructor("BDSParameterisationPhysics"),
  activated(false),
  fastSimulationManagerProcess(nullptr)
{;}

void BDSParameterisationPhysics::ConstructParticle()
{
  G4Gamma::Gamma();
  G4Electron::Electron();
  G4Positron::Positron();    
  return;
}

void BDSParameterisationPhysics::ConstructProcess()
{
  if(activated)
    {return;}
  activated=true;
  
  fastSimulationManagerProcess = new G4FastSimulationManagerProcess();
  
#if G4VERSION_NUMBER > 1029
  auto aParticleIterator = GetParticleIterator();
#endif
  aParticleIterator->reset();

  while( (*aParticleIterator)() )
    {
      G4ParticleDefinition* particle = aParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
      pmanager->AddDiscreteProcess(fastSimulationManagerProcess);
    }
  return;
}


