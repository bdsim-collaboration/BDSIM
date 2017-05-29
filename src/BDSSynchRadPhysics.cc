#include "BDSSynchRadPhysics.hh"

#include "globals.hh" // geant4 types / globals
#include "G4AutoDelete.hh"
#include "G4Gamma.hh"
#include "G4LeptonConstructor.hh"
#include "G4OpticalPhoton.hh"
#include "G4ParticleDefinition.hh"
#include "G4SynchrotronRadiation.hh"
#include "G4Version.hh"

BDSSynchRadPhysics::BDSSynchRadPhysics():
  G4VPhysicsConstructor("BDSSynchRadPhysics")
{;}

BDSSynchRadPhysics::~BDSSynchRadPhysics()
{;}

void BDSSynchRadPhysics::ConstructParticle()
{
  G4LeptonConstructor leptons;
  leptons.ConstructParticle();

  G4Gamma::Gamma();
  G4OpticalPhoton::OpticalPhotonDefinition();
}

void BDSSynchRadPhysics::ConstructProcess()
{
  G4SynchrotronRadiation* synchrotron = new G4SynchrotronRadiation();
  G4AutoDelete::Register(synchrotron);
  
  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
  
#if G4VERSION_NUMBER > 1029
  auto aParticleIterator = GetParticleIterator();
#endif
  aParticleIterator->reset();
  while( (*aParticleIterator)() )
    {
      G4ParticleDefinition* particle = aParticleIterator->value(); 
      // add to charged particles
      if (particle->GetPDGCharge() != 0)
	{ph->RegisterProcess(synchrotron, particle);}
    }
  return;
}


