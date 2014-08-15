#include "BDSMuonPhysics.hh"
#include "BDSXSBiasPhysics.hh"
#include "G4GammaConversionToMuons.hh"
#include "BDSVProcess.hh"
#include "BDSGlobalConstants.hh"
#include "G4eeToHadrons.hh"
#include "G4Version.hh"

BDSMuonPhysics::BDSMuonPhysics():BDSXSBiasPhysics("BDSMuonPhysics")
{;}

BDSMuonPhysics::~BDSMuonPhysics()
{;}

void BDSMuonPhysics::ConstructParticle(){
  G4Gamma::Gamma();
  G4Electron::Electron();
  G4Positron::Positron();    
  G4MuonPlus::MuonPlus();
  G4MuonMinus::MuonMinus();
  G4PionPlus::PionPlusDefinition();
  G4PionMinus::PionMinusDefinition();
  return;
}

void BDSMuonPhysics::ConstructProcess(){
  if(_wasActivated) return;
  _wasActivated=true;
  
#if G4VERSION_NUMBER < 1000
  theParticleTable = G4ParticleTable::GetParticleTable();
  theParticleIterator = theParticleTable->GetIterator();
  G4ParticleTable::G4PTblDicIterator* aParticleIterator = theParticleIterator;
#endif

  aParticleIterator->reset();

  while( (*aParticleIterator)() ){
    G4ParticleDefinition* particle = aParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    if(particleName == "gamma"){
      G4GammaConversionToMuons* gamConvToMu = new G4GammaConversionToMuons();
      ReplaceDiscreteProcess(pmanager, gamConvToMu, WrapXSBias(gamConvToMu,BDSGlobalConstants::Instance()->GetGammaToMuFe()));
    }      
    if(particleName == "e+"){
      G4AnnihiToMuPair* anni = new G4AnnihiToMuPair();
      ReplaceDiscreteProcess(pmanager, anni, WrapXSBias(anni,BDSGlobalConstants::Instance()->GetAnnihiToMuFe()));
      G4eeToHadrons* eetohadrons = new G4eeToHadrons();
      ReplaceDiscreteProcess(pmanager, eetohadrons, WrapXSBias(eetohadrons,BDSGlobalConstants::Instance()->GetEeToHadronsFe()));
    }      
  }
  return;
}

//Returns the wrapper process.
BDSXSBias* BDSMuonPhysics::WrapXSBias(G4GammaConversionToMuons* proc, G4double efactor){
  proc->SetCrossSecFactor(efactor);
  BDSXSBias* xsbias_proc = new BDSXSBias();
  xsbias_proc->RegisterProcess(proc);
  xsbias_proc->eFactor(efactor);
  return xsbias_proc;
}

BDSXSBias* BDSMuonPhysics::WrapXSBias(G4AnnihiToMuPair* proc, G4double efactor){
  proc->SetCrossSecFactor(efactor);
  BDSXSBias* xsbias_proc = new BDSXSBias();
  xsbias_proc->RegisterProcess(proc);
  xsbias_proc->eFactor(efactor);
  return xsbias_proc;
}

BDSXSBias* BDSMuonPhysics::WrapXSBias(G4eeToHadrons* proc, G4double efactor){
  proc->SetCrossSecFactor(efactor);
  BDSXSBias* xsbias_proc = new BDSXSBias();
  xsbias_proc->RegisterProcess(proc);
  xsbias_proc->eFactor(efactor);
  return xsbias_proc;
}

