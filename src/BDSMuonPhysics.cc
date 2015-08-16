#include "BDSExecOptions.hh"
#include "BDSMuonPhysics.hh"
#include "BDSXSBiasPhysics.hh"
#include "BDSVProcess.hh"
#include "BDSGlobalConstants.hh"

#include "G4GammaConversionToMuons.hh"
#include "G4eeToHadrons.hh"
#include "G4Version.hh"
#include "G4PhysicsListHelper.hh"

BDSMuonPhysics::BDSMuonPhysics() : BDSXSBiasPhysics("BDSMuonPhysics")
{
  verbose = BDSExecOptions::Instance()->GetVerbose();
  
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
}

BDSMuonPhysics::~BDSMuonPhysics()
{;}

void BDSMuonPhysics::ConstructParticle()
{
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
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
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;

  if(_wasActivated) return;
  _wasActivated=true;
  
#if G4VERSION_NUMBER < 1000
  theParticleTable = G4ParticleTable::GetParticleTable();
  theParticleIterator = theParticleTable->GetIterator();
  G4ParticleTable::G4PTblDicIterator* aParticleIterator = theParticleIterator;
#endif

  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

  aParticleIterator->reset();

  while( (*aParticleIterator)() ){
    G4ParticleDefinition* particle = aParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();

    if(particleName == "gamma"){
      G4GammaConversionToMuons* gamConvToMu = new G4GammaConversionToMuons();
      ph->RegisterProcess(gamConvToMu,particle);
      //      ReplaceDiscreteProcess(pmanager, gamConvToMu, WrapXSBias(gamConvToMu,BDSGlobalConstants::Instance()->GetGammaToMuFe()));
    }      
    if(particleName == "e+"){
      G4AnnihiToMuPair* anni = new G4AnnihiToMuPair();
      ph->RegisterProcess(anni,particle);
      //      ReplaceDiscreteProcess(pmanager, anni, WrapXSBias(anni,BDSGlobalConstants::Instance()->GetAnnihiToMuFe()));
      G4eeToHadrons* eetohadrons = new G4eeToHadrons();
      ph->RegisterProcess(eetohadrons,particle);
      //      ReplaceDiscreteProcess(pmanager, eetohadrons, WrapXSBias(eetohadrons,BDSGlobalConstants::Instance()->GetEeToHadronsFe()));
    }      
  }
  return;
}

//Returns the wrapper process.
BDSXSBias* BDSMuonPhysics::WrapXSBias(G4GammaConversionToMuons* proc, G4double efactor){
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;

  proc->SetCrossSecFactor(efactor);
  BDSXSBias* xsbias_proc = new BDSXSBias();
  xsbias_proc->RegisterProcess(proc);
  xsbias_proc->eFactor(efactor);
  return xsbias_proc;
}

BDSXSBias* BDSMuonPhysics::WrapXSBias(G4AnnihiToMuPair* proc, G4double efactor){
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;

  proc->SetCrossSecFactor(efactor);
  BDSXSBias* xsbias_proc = new BDSXSBias();
  xsbias_proc->RegisterProcess(proc);
  xsbias_proc->eFactor(efactor);
  return xsbias_proc;
}

BDSXSBias* BDSMuonPhysics::WrapXSBias(G4eeToHadrons* proc, G4double efactor){
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;

  proc->SetCrossSecFactor(efactor);
  BDSXSBias* xsbias_proc = new BDSXSBias();
  xsbias_proc->RegisterProcess(proc);
  xsbias_proc->eFactor(efactor);
  return xsbias_proc;
}

