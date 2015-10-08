#include "BDSModularPhysicsList.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMuonPhysics.hh"
#include "BDSSynchRadPhysics.hh"
#include "BDSParameterisationPhysics.hh"
#include "BDSExecOptions.hh"

#include "G4EmPenelopePhysics.hh"
#include "G4OpticalPhysics.hh"
#include "G4OpticalProcessIndex.hh"
#include "G4ParticleTable.hh"
#include "G4EmStandardPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Proton.hh"
#include "G4AntiProton.hh"

#include "G4Version.hh"
#if G4VERSION_NUMBER < 1000
#include "HadronPhysicsQGSP_BERT.hh"
#include "HadronPhysicsQGSP_BERT_HP.hh"
#else
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BERT.hh"
#endif

//Note: transportation process is constructed by default with classes that derive from G4VModularPhysicsList

BDSModularPhysicsList::BDSModularPhysicsList():
  G4VModularPhysicsList(),
  physListName(BDSGlobalConstants::Instance()->GetPhysListName())
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  verbose = BDSExecOptions::Instance()->GetVerbose();
  globals = BDSGlobalConstants::Instance();
  
  SetVerboseLevel(1);
  emPhysics       = nullptr;
  hadronicPhysics = nullptr;
  muonPhysics     = nullptr;
  opticalPhysics  = nullptr;
  decayPhysics    = nullptr;
  paramPhysics    = nullptr;
  synchRadPhysics = nullptr;
  cutsAndLimits   = nullptr;
  
  ParsePhysicsList();
  ConfigurePhysics();
  Register();
  ConstructMinimumParticleSet();
  SetParticleDefinition();
  SetCuts();
  DumpCutValuesTable(100);
}


BDSModularPhysicsList::~BDSModularPhysicsList()
{;}

void BDSModularPhysicsList::Print()
{;}

//Parse the physicsList option
void BDSModularPhysicsList::ParsePhysicsList()
{
  if(physListName.contains((G4String)"em"))
    {
      if(physListName.contains((G4String)"emlow"))
	{LoadEmLow();}
      else       
	{LoadEm();}
    }
  
  //Hadronic
  if(physListName.contains((G4String)"hadronic"))
    {
      if(!emPhysics)
	{LoadEm();}
      if(physListName.contains((G4String)"hadronichp"))
	{LoadHadronicHP();}
      else
	{LoadHadronic();}
    }
  
  //Synchrotron radiation
  if(physListName.contains((G4String)"synchrad"))
    {
    if(!emPhysics)
      {LoadEm();}
    LoadSynchRad();
    }

  //Cross section biased muon production
  if(physListName.contains((G4String)"muon"))
    {
      //Must construct em physics first
      if(!emPhysics)
	{LoadEm();}
      if(!hadronicPhysics)
	{LoadHadronic();}
      LoadMuon();
    }

  if(physListName.contains((G4String)"optical"))
    {LoadOptical();} //Optical
  
  //Decay - constructed by default if hadronic or em are constructed, 
  //unless "nodecay"
  if(hadronicPhysics || emPhysics)
    {
      if(!physListName.contains((G4String)"nodecay"))
	{LoadDecay();}
  }
  
  //Always load cuts and limits.
  LoadCutsAndLimits();
}

void BDSModularPhysicsList::ConstructMinimumParticleSet()
{
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  //Minimum required set of particles required for tracking
  G4Gamma::Gamma();
  G4Electron::Electron();
  G4Positron::Positron();
  G4Proton::Proton();
  G4AntiProton::AntiProton();
}

void BDSModularPhysicsList::ConfigurePhysics()
{
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  if(opticalPhysics)
    {ConfigureOptical();}
}

void BDSModularPhysicsList::ConfigureOptical()
{
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  if (!opticalPhysics) return;
  opticalPhysics->Configure(kCerenkov,      globals->GetTurnOnCerenkov());           ///< Cerenkov process index                                   
  opticalPhysics->Configure(kScintillation, true);                                   ///< Scintillation process index                              
  opticalPhysics->Configure(kAbsorption,    globals->GetTurnOnOpticalAbsorption());  ///< Absorption process index                                 
  opticalPhysics->Configure(kRayleigh,      globals->GetTurnOnRayleighScattering()); ///< Rayleigh scattering process index                        
  opticalPhysics->Configure(kMieHG,         globals->GetTurnOnMieScattering());      ///< Mie scattering process index                             
  opticalPhysics->Configure(kBoundary,      globals->GetTurnOnOpticalSurface());     ///< Boundary process index                                   
  opticalPhysics->Configure(kWLS,           true);                                    ///< Wave Length Shifting process index                       
// opticalPhysics->Configure(kNoProcess,      globals->GetTurnOn< Number of processes, no selected process
  opticalPhysics->SetScintillationYieldFactor(globals->GetScintYieldFactor());
}

void BDSModularPhysicsList::Register()
{
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  for(auto physics : constructors)
    {RegisterPhysics(physics);}
}

void BDSModularPhysicsList::SetCuts()
{
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;

  G4VUserPhysicsList::SetCuts();  
  G4double defaultRangeCut  = globals->GetDefaultRangeCut(); 
  SetDefaultCutValue(defaultRangeCut);
  SetCutsWithDefault();   

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Default production range cut (mm)   " << defaultRangeCut   << G4endl;
#endif

  G4double prodCutPhotons   = globals->GetProdCutPhotons();
  G4double prodCutElectrons = globals->GetProdCutElectrons();
  G4double prodCutPositrons = globals->GetProdCutPositrons();
  G4double prodCutProtons   = globals->GetProdCutProtons();  

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Photon production range cut (mm)   " << prodCutPhotons   << G4endl;
  G4cout << __METHOD_NAME__ << "Electron production range cut (mm) " << prodCutElectrons << G4endl;
  G4cout << __METHOD_NAME__ << "Positron production range cut (mm) " << prodCutPositrons << G4endl;
  G4cout << __METHOD_NAME__ << "Proton production range cut (mm)   " << prodCutProtons<< G4endl;
#endif
  
  SetCutValue(prodCutPhotons,  "gamma");
  SetCutValue(prodCutElectrons,"e-");
  SetCutValue(prodCutPositrons,"e+");
  SetCutValue(prodCutProtons,  "proton");

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "list of all constructed particles by physics lists" << G4endl;
  for (auto particle : *G4ParticleTable::fDictionary)
    {G4cout << particle.second->GetParticleName() << " ";}
  G4cout << G4endl;
#endif
  
  DumpCutValuesTable(); 
}  

void BDSModularPhysicsList::SetParticleDefinition()
{
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;

  // set primary particle definition and kinetic beam parameters other than total energy
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  globals->SetParticleDefinition(particleTable->FindParticle(globals->GetParticleName()));  
  
  if(!globals->GetParticleDefinition()) 
    {G4Exception("Particle not found, quitting!", "-1", FatalException, ""); exit(1);}
  
  // set kinetic beam parameters other than total energy
  globals->SetBeamMomentum(sqrt(pow(globals->GetBeamTotalEnergy(),2)-pow(globals->GetParticleDefinition()->GetPDGMass(),2)));
  globals->SetBeamKineticEnergy(globals->GetBeamTotalEnergy()-globals->GetParticleDefinition()->GetPDGMass());
  globals->SetParticleMomentum(sqrt(pow(globals->GetParticleTotalEnergy(),2)-pow(globals->GetParticleDefinition()->GetPDGMass(),2)));
  globals->SetParticleKineticEnergy(globals->GetParticleTotalEnergy()-globals->GetParticleDefinition()->GetPDGMass());
  
  G4cout << __METHOD_NAME__ << "Beam properties:"<<G4endl;
  G4cout << __METHOD_NAME__ << "Particle : " 
	 << globals->GetParticleDefinition()->GetParticleName()<<G4endl;
  G4cout << __METHOD_NAME__ << "Mass : " 
	 << globals->GetParticleDefinition()->GetPDGMass()/CLHEP::GeV<< " GeV"<<G4endl;
  G4cout << __METHOD_NAME__ << "Charge : " 
	 << globals->GetParticleDefinition()->GetPDGCharge()<< " e"<<G4endl;
  G4cout << __METHOD_NAME__ << "Total Energy : "
	 << globals->GetBeamTotalEnergy()/CLHEP::GeV<<" GeV"<<G4endl;
  G4cout << __METHOD_NAME__ << "Kinetic Energy : "
	 << globals->GetBeamKineticEnergy()/CLHEP::GeV<<" GeV"<<G4endl;
  G4cout << __METHOD_NAME__ << "Momentum : "
	 << globals->GetBeamMomentum()/CLHEP::GeV<<" GeV"<<G4endl;
}

void BDSModularPhysicsList::LoadEm()
{
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  if(!emPhysics)
    {
      emPhysics = new G4EmStandardPhysics();		  
      constructors.push_back(emPhysics);			  
      LoadParameterisationPhysics();	
    }	  
}							  
							  
void BDSModularPhysicsList::LoadEmLow()
{
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;			  
  if(!emPhysics)
    {
      emPhysics = new G4EmPenelopePhysics();		  
      constructors.push_back(emPhysics);			  
      LoadParameterisationPhysics();			  
    }
}							  
							  
void BDSModularPhysicsList::LoadParameterisationPhysics()
{
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  if(!paramPhysics)
    {
      paramPhysics = new BDSParameterisationPhysics();	  
      constructors.push_back(paramPhysics);		  
    }
}							  
							  
void BDSModularPhysicsList::LoadHadronic()
{
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  if(!hadronicPhysics)
    {
#if G4VERSION_NUMBER < 1000
      hadronicPhysics = new HadronPhysicsQGSP_BERT();
#else
      hadronicPhysics = new G4HadronPhysicsQGSP_BERT();
#endif
      constructors.push_back(hadronicPhysics);		  
    }
}							  
							  
void BDSModularPhysicsList::LoadHadronicHP()
{
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  if(!hadronicPhysics)
    {
#if G4VERSION_NUMBER < 1000
      hadronicPhysics = new HadronPhysicsQGSP_BERT_HP();
#else
      hadronicPhysics = new G4HadronPhysicsQGSP_BERT_HP();
#endif
      constructors.push_back(hadronicPhysics);		  
    }
}							  
							  
void BDSModularPhysicsList::LoadSynchRad()
{		    
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  if(!synchRadPhysics)
    {
      synchRadPhysics = new BDSSynchRadPhysics();		  
      constructors.push_back(synchRadPhysics);		  
    }
  // Switch on BDSGlobalConstants::SetSynchRadOn() to keep BDSPhysicsListCompatibility
  globals->SetSynchRadOn(true);
}							  
							  
void BDSModularPhysicsList::LoadMuon()
{			  
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  if(!muonPhysics)
    {
      muonPhysics = new BDSMuonPhysics();			  
      constructors.push_back(muonPhysics);		  
    }
}							  
							  
void BDSModularPhysicsList::LoadOptical()
{
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  if(!opticalPhysics)
    {
      opticalPhysics = new G4OpticalPhysics();		  
      constructors.push_back(opticalPhysics);		  
    }
}							  
							  
void BDSModularPhysicsList::LoadDecay()
{			  
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  if(!decayPhysics)
    {
      decayPhysics = new G4DecayPhysics();			  
      constructors.push_back(decayPhysics);		  
    }
}                                                         

void BDSModularPhysicsList::LoadCutsAndLimits()
{
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  if(!cutsAndLimits)
    {
      cutsAndLimits = new BDSCutsAndLimits();			  
      constructors.push_back(cutsAndLimits);		  
    }
}                                                         
