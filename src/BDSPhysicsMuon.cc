#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSPhysicsMuon.hh"

#include "G4AutoDelete.hh"
#include "G4AnnihiToMuPair.hh"
#include "G4eeToHadrons.hh"
#include "G4Gamma.hh"
#include "G4GammaConversionToMuons.hh"
#include "G4LeptonConstructor.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuIonisation.hh"
#include "G4MuMultipleScattering.hh"
#include "G4MuPairProduction.hh"
#include "G4OpticalPhoton.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysicsListHelper.hh"
#include "G4PionDecayMakeSpin.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4Version.hh"


BDSPhysicsMuon::BDSPhysicsMuon():
  G4VPhysicsConstructor("BDSPhysicsMuon")
{;}

BDSPhysicsMuon::~BDSPhysicsMuon()
{;}

void BDSPhysicsMuon::ConstructParticle()
{ 
  // leptons
  G4LeptonConstructor leptons;
  leptons.ConstructParticle();

  // photons
  G4Gamma::Gamma();
  G4OpticalPhoton::OpticalPhoton();
  
  // pions
  G4PionPlus::PionPlus();
  G4PionMinus::PionMinus();
}

void BDSPhysicsMuon::ConstructProcess()
{
  // for gamma
  G4GammaConversionToMuons* gammaToMuPair = new G4GammaConversionToMuons();
  G4AutoDelete::Register(gammaToMuPair);

  // for e+
  G4AnnihiToMuPair* ePlusToMuPair = new G4AnnihiToMuPair();
  G4AutoDelete::Register(ePlusToMuPair);
  G4eeToHadrons*    eeToHadrons   = new G4eeToHadrons();
  G4AutoDelete::Register(eeToHadrons);

  // for muon +-
  G4MuMultipleScattering* mumsc = new G4MuMultipleScattering();
  G4AutoDelete::Register(mumsc);
  G4MuIonisation*         muion = new G4MuIonisation();
  G4AutoDelete::Register(muion);
  G4MuBremsstrahlung*     mubrm = new G4MuBremsstrahlung();
  G4AutoDelete::Register(mubrm);
  G4MuPairProduction*     mupar = new G4MuPairProduction();
  G4AutoDelete::Register(mupar);
  
  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
  
#if G4VERSION_NUMBER > 1029
  auto aParticleIterator = GetParticleIterator();
#endif
  aParticleIterator->reset();

  while( (*aParticleIterator)() )
    {
      G4ParticleDefinition* particle = aParticleIterator->value();
      G4String particleName = particle->GetParticleName();
      
      if(particleName == "gamma")
	{
	  ph->RegisterProcess(gammaToMuPair, particle);
	  continue;
	}
      if(particleName == "e+")
	{
	  ph->RegisterProcess(ePlusToMuPair, particle);
	  ph->RegisterProcess(eeToHadrons,   particle);
	  continue;
	}
      if(particleName == "mu+" || particleName == "mu-")
	{
	  ph->RegisterProcess(mumsc, particle);
	  ph->RegisterProcess(muion, particle);
	  ph->RegisterProcess(mubrm, particle);
	  ph->RegisterProcess(mupar, particle);
	  continue;
	}
    }
}
