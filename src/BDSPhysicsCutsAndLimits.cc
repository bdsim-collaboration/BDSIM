#include "BDSPhysicsCutsAndLimits.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Proton.hh"
#include "G4StepLimiter.hh"
#include "G4UserSpecialCuts.hh"
#include "G4Version.hh"

BDSPhysicsCutsAndLimits::BDSPhysicsCutsAndLimits():
  G4VPhysicsConstructor("BDSPhysicsCutsAndLimits")
{
  stepLimiter = new G4StepLimiter;
  specialCuts = new G4UserSpecialCuts;
}

BDSPhysicsCutsAndLimits::~BDSPhysicsCutsAndLimits()
{
  delete stepLimiter;
  delete specialCuts;
}

void BDSPhysicsCutsAndLimits::ConstructParticle()
{
  G4Gamma::Gamma();
  G4Electron::Electron();
  G4Positron::Positron();    
  G4Proton::Proton();
}

void BDSPhysicsCutsAndLimits::ConstructProcess()
{
  if (Activated())
    {return;}

  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

#if G4VERSION_NUMBER > 1029
  auto aParticleIterator = GetParticleIterator();
#endif
  aParticleIterator->reset();
  while( (*aParticleIterator)())
    {
      G4ParticleDefinition* particle = aParticleIterator->value();
      
      if((particle->GetParticleName()=="gamma")||
	 (particle->GetParticleName()=="e-")||
	 (particle->GetParticleName()=="e+")||
	 (particle->GetParticleName()=="proton"))
	{particle->SetApplyCutsFlag(true);}
      ph->RegisterProcess(stepLimiter,particle); // this is for MaxAllowedStep
      ph->RegisterProcess(specialCuts,particle); // this is for all other limits
    }

  SetActivated();
}

