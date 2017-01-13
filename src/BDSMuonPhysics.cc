#include "BDSGlobalConstants.hh"
#include "BDSMuonPhysics.hh"

#include "G4AnnihiToMuPair.hh"
#include "G4Cerenkov.hh"
#include "G4eeToHadrons.hh"
#include "G4EmSaturation.hh"
#include "G4Gamma.hh"
#include "G4GammaConversionToMuons.hh"
#include "G4LeptonConstructor.hh"
#include "G4LossTableManager.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuIonisation.hh"
#include "G4MuMultipleScattering.hh"
#include "G4MuPairProduction.hh"
#include "G4OpticalPhoton.hh"
#include "G4ParticleDefinition.hh"
#include "G4PionDecayMakeSpin.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4ProcessManager.hh"
#include "G4Scintillation.hh"
#include "G4Version.hh"

// TODO : use G4PhysicsListHelper to set ordering of these processes (just to be sure)

BDSMuonPhysics::BDSMuonPhysics():
  G4VPhysicsConstructor("BDSMuonPhysics")
{
  verbose = BDSGlobalConstants::Instance()->Verbose();
  
  if(verbose || debug) 
    {G4cout << __METHOD_NAME__ << G4endl;}
}

BDSMuonPhysics::~BDSMuonPhysics()
{;}

void BDSMuonPhysics::ConstructParticle()
{
  if(verbose || debug) 
    {G4cout << __METHOD_NAME__ << G4endl;}
  // leptons
  G4LeptonConstructor leptons;
  leptons.ConstructParticle();

  // photons
  G4Gamma::Gamma();
  G4OpticalPhoton::OpticalPhoton();
  
  //pions
  G4PionPlus::PionPlus();
  G4PionMinus::PionMinus();
}

void BDSMuonPhysics::ConstructProcess()
{
  if(verbose || debug) 
    {G4cout << __METHOD_NAME__ << G4endl;}

  G4Cerenkov*        cer = nullptr;
  G4Scintillation* scint = nullptr;
  if (BDSGlobalConstants::Instance()->TurnOnCerenkov())
  {
    cer = new G4Cerenkov();
    // reduce memory profile
    cer->SetTrackSecondariesFirst(true);
    // common settings (similar to optical physics) 
    cer->SetMaxNumPhotonsPerStep(100);
    cer->SetMaxBetaChangePerStep(10.0);
    
    // Cerenkov also needs scintillation it seems
    scint = new G4Scintillation();
    scint->SetScintillationYieldFactor(BDSGlobalConstants::Instance()->ScintYieldFactor());
    scint->SetTrackSecondariesFirst(true);
    G4EmSaturation* emSaturation = G4LossTableManager::Instance()->EmSaturation();
    scint->AddSaturation(emSaturation);
  }
  
#if G4VERSION_NUMBER > 1029
  auto aParticleIterator = GetParticleIterator();
#endif
  aParticleIterator->reset();

  while( (*aParticleIterator)() )
  {
    G4ParticleDefinition* particle = aParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
      
    if(particleName == "gamma")
    {
      G4GammaConversionToMuons* gamConvToMu = new G4GammaConversionToMuons();
      pmanager->AddDiscreteProcess(gamConvToMu);
    }
    if(particleName == "e+")
    {
      // Processes for positron annihilation
	    G4AnnihiToMuPair* anni = new G4AnnihiToMuPair();
      pmanager->AddDiscreteProcess(anni);
      G4eeToHadrons* eetohadrons = new G4eeToHadrons();
      pmanager->AddDiscreteProcess(eetohadrons);
    }
    if(particleName == "pi+" || particleName == "pi-") 
    {
      G4PionDecayMakeSpin *pdms = new G4PionDecayMakeSpin();
      pmanager->AddDiscreteProcess(pdms);
    }
    if(particleName == "mu+" || particleName == "mu-")
    {
      G4MuMultipleScattering* mumsc = new G4MuMultipleScattering();
      pmanager->AddProcess(mumsc);
      G4MuIonisation*         muion = new G4MuIonisation();
      pmanager->AddProcess(muion);
      G4MuBremsstrahlung*     mubrm = new G4MuBremsstrahlung();
      pmanager->AddProcess(mubrm);
      G4MuPairProduction*     mupar = new G4MuPairProduction();
      pmanager->AddProcess(mupar);
    }

    // turn on Cerenkov for all particles, needed for Geant4 10.1
    if (BDSGlobalConstants::Instance()->TurnOnCerenkov())
    {
      if (cer->IsApplicable(*particle))
      {
        pmanager->AddProcess(cer);
        pmanager->SetProcessOrdering(cer,idxPostStep);
        // Cerenkov also needs scintillation it seems, for 10.1 and 10.2
	      pmanager->AddProcess(scint);
        pmanager->SetProcessOrderingToLast(scint,idxAtRest);
        pmanager->SetProcessOrderingToLast(scint,idxPostStep);
	    }
    }
  }
}

