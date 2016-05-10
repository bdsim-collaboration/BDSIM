#include "BDSGlobalConstants.hh"
#include "BDSMuonPhysics.hh"

#include "G4AnnihiToMuPair.hh"
#include "G4Cerenkov.hh"
#include "G4eeToHadrons.hh"
#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4GammaConversionToMuons.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuIonisation.hh"
#include "G4MuMultipleScattering.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4MuPairProduction.hh"
#include "G4ParticleDefinition.hh"
#include "G4PionDecayMakeSpin.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4Positron.hh"
#include "G4ProcessManager.hh"

BDSMuonPhysics::BDSMuonPhysics():
  G4VPhysicsConstructor("BDSMuonPhysics"),
  activated(false)
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
  G4Gamma::Gamma();
  G4Electron::Electron();
  G4Positron::Positron();    
  G4MuonPlus::MuonPlus();
  G4MuonMinus::MuonMinus();
  G4PionPlus::PionPlusDefinition();
  G4PionMinus::PionMinusDefinition();
  return;
}

void BDSMuonPhysics::ConstructProcess()
{
  if(verbose || debug) 
    {G4cout << __METHOD_NAME__ << G4endl;}

  if(activated)
    {return;}
  activated=true;
  
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
	G4Cerenkov*             mucer = new G4Cerenkov();
	// reduce memory profile
	mucer->SetTrackSecondariesFirst(true);
	pmanager->AddProcess(mucer);
	pmanager->SetProcessOrdering(mucer,idxPostStep);
      }
    }
  return;
}

