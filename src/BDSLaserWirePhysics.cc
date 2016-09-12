#include "BDSLaserCompton.hh"
#include "BDSLaserWirePhysics.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4ParticleDefinition.hh"
#include "G4Positron.hh"
#include "G4ProcessManager.hh"


BDSLaserWirePhysics::BDSLaserWirePhysics():
  G4VPhysicsConstructor("BDSLaserWirePhysics")
{;}

BDSLaserWirePhysics::~BDSLaserWirePhysics()
{;}

void BDSLaserWirePhysics::ConstructParticle()
{
  G4Electron::ElectronDefinition();
  G4Positron::PositronDefinition();
  G4Gamma::Gamma();
}

void BDSLaserWirePhysics::ConstructProcess()
{
  aParticleIterator->reset();

  BDSLaserCompton* lwProcess = new BDSLaserCompton();

  while((*aParticleIterator)())
    {
      G4ParticleDefinition* particle = aParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
      G4String particleName = particle->GetParticleName();
      
      if (particleName == "e-")
	{
	  pmanager->AddProcess(lwProcess);
	  pmanager->SetProcessOrderingToLast(lwProcess,idxPostStep);
	}

      if (particleName == "e+")
	{
	  pmanager->AddProcess(lwProcess);
	  pmanager->SetProcessOrderingToLast(lwProcess,idxPostStep);
	}
    }
}
