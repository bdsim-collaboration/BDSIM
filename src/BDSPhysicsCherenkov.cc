#include "BDSPhysicsCherenkov.hh"

#include "globals.hh" // geant4 types / globals
#include "G4AutoDelete.hh"
#include "G4Cerenkov.hh"
#include "G4LeptonConstructor.hh"
#include "G4OpticalPhoton.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysicsListHelper.hh"
#include "G4Version.hh"

BDSPhysicsCherenkov::BDSPhysicsCherenkov(G4double maxPhotonsPerStepIn,
					 G4double maxBetaChangePerStepIn):
  G4VPhysicsConstructor("BDSPhysicsCherenkov"),
  maxPhotonsPerStep(maxPhotonsPerStepIn),
  maxBetaChangePerStep(maxBetaChangePerStepIn)
{;}

BDSPhysicsCherenkov::~BDSPhysicsCherenkov()
{;}

void BDSPhysicsCherenkov::ConstructParticle()
{
  G4LeptonConstructor leptons;
  leptons.ConstructParticle();

  G4OpticalPhoton::OpticalPhotonDefinition();
}

void BDSPhysicsCherenkov::ConstructProcess()
{
  G4Cerenkov* cherenkov = new G4Cerenkov();
  G4AutoDelete::Register(cherenkov);

  // reduce memory profile
  cherenkov->SetTrackSecondariesFirst(true);

  // common settings (similar to optical physics)
  if (maxPhotonsPerStep > 0)
    {cherenkov->SetMaxNumPhotonsPerStep(maxPhotonsPerStep);}
  cherenkov->SetMaxBetaChangePerStep(maxBetaChangePerStep);

  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
    
#if G4VERSION_NUMBER > 1029
  auto aParticleIterator = GetParticleIterator();
#endif
  aParticleIterator->reset();
  while( (*aParticleIterator)() )
    {
      G4ParticleDefinition* particle = aParticleIterator->value();
      
      if (cherenkov->IsApplicable(*particle))
	{ph->RegisterProcess(cherenkov, particle);}
    }
}
