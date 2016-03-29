#include "BDSSynchRadPhysics.hh"

#include "globals.hh" // geant4 types / globals
#include "G4AutoDelete.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4SynchrotronRadiation.hh"

BDSSynchRadPhysics::BDSSynchRadPhysics():
  G4VPhysicsConstructor("BDSSynchRadPhysics"),
  activated(false)
{;}

BDSSynchRadPhysics::~BDSSynchRadPhysics()
{;}

void BDSSynchRadPhysics::ConstructParticle()
{return;}

void BDSSynchRadPhysics::ConstructProcess()
{
  if(activated)
    {return;}
  activated = true;

  G4SynchrotronRadiation* synchrotron = new G4SynchrotronRadiation();
  G4AutoDelete::Register(synchrotron);
  
  aParticleIterator->reset();
  while( (*aParticleIterator)() )
    {
      G4ParticleDefinition* particle = aParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();

      // add to charged particles
      if (particle->GetPDGCharge() != 0)
	{
	  pmanager->AddProcess(synchrotron);
	  pmanager->SetProcessOrderingToLast(synchrotron,idxPostStep);
	}
    }
  return;
}


