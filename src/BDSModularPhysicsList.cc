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

//Note: transportation process is constructed by default with classes derive from G4VModularPhysicsList

BDSModularPhysicsList::BDSModularPhysicsList():
  G4VModularPhysicsList(),
  _physListName(BDSGlobalConstants::Instance()->GetPhysListName())
{
  SetVerboseLevel(1);
  _emPhysics=nullptr;
  _hadronicPhysics=nullptr;
  _muonPhysics=nullptr;
  _opticalPhysics=nullptr;
  _decayPhysics=nullptr;
  _paramPhysics=nullptr;
  _synchRadPhysics=nullptr;
  _cutsAndLimits=nullptr;
  
  ParsePhysicsList();
  ConfigurePhysics();
  Register();
  ConstructMinimumParticleSet();
  SetParticleDefinition();
  SetCuts();
  DumpCutValuesTable(100);

  verbose = BDSExecOptions::Instance()->GetVerbose();
}


BDSModularPhysicsList::~BDSModularPhysicsList()
{;}

void BDSModularPhysicsList::Print()
{;}

//Parse the physicsList option
void BDSModularPhysicsList::ParsePhysicsList()
{
  if(_physListName.contains((G4String)"em")){
    if(_physListName.contains((G4String)"emlow")){
      LoadEmLow();
    } else       
      LoadEm();
  }
  
  //Hadronic
  if(_physListName.contains((G4String)"hadronic")){
    if(!_emPhysics){
      LoadEm();
    }
    if(_physListName.contains((G4String)"hadronichp")){
      LoadHadronicHP();
    }else{
      LoadHadronic();
    }
  }
  
  //Synchrotron radiation
  if(_physListName.contains((G4String)"synchrad")){
    if(!_emPhysics){
      LoadEm();
    }
    LoadSynchRad();
  }

  //Cross section biased muon production
  if(_physListName.contains((G4String)"muon")){
    //Must construct em physics first
    if(!_emPhysics){
      LoadEm();
    }
    if(!_hadronicPhysics){
      LoadHadronic();
    }
    LoadMuon();
  }

  if(_physListName.contains((G4String)"optical")){
    //Optical
    LoadOptical();
  }
  //Decay - constructed by default if hadronic or em are constructed, 
  //unless "nodecay"
  if(_hadronicPhysics || _emPhysics){
    if(!_physListName.contains((G4String)"nodecay")){
      LoadDecay();
    }
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
  if(_opticalPhysics){ ConfigureOptical();}
}

void BDSModularPhysicsList::ConfigureOptical()
{
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  if (!_opticalPhysics) return;
  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  _opticalPhysics->Configure(kCerenkov,      globals->GetTurnOnCerenkov());           ///< Cerenkov process index                                   
  _opticalPhysics->Configure(kScintillation, true);                                   ///< Scintillation process index                              
  _opticalPhysics->Configure(kAbsorption,    globals->GetTurnOnOpticalAbsorption());  ///< Absorption process index                                 
  _opticalPhysics->Configure(kRayleigh,      globals->GetTurnOnRayleighScattering()); ///< Rayleigh scattering process index                        
  _opticalPhysics->Configure(kMieHG,         globals->GetTurnOnMieScattering());      ///< Mie scattering process index                             
  _opticalPhysics->Configure(kBoundary,      globals->GetTurnOnOpticalSurface());     ///< Boundary process index                                   
  _opticalPhysics->Configure(kWLS,           true);                                    ///< Wave Length Shifting process index                       
// _opticalPhysics->Configure(kNoProcess,      globals->GetTurnOn< Number of processes, no selected process
  _opticalPhysics->SetScintillationYieldFactor(globals->GetScintYieldFactor());
}

void BDSModularPhysicsList::Register()
{
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  std::vector<G4VPhysicsConstructor*>::iterator it;
  for(it = _constructors.begin(); it != _constructors.end(); it++){
    RegisterPhysics(*it);
  }
}

void BDSModularPhysicsList::SetCuts()
{
  if(verbose || debug) 
    {G4cout << __METHOD_NAME__ << G4endl;}

  BDSGlobalConstants* globalConsts = BDSGlobalConstants::Instance();

  G4VUserPhysicsList::SetCuts();  
  G4double defaultRangeCut  = globalConsts->GetDefaultRangeCut(); 
  SetDefaultCutValue(defaultRangeCut);
  SetCutsWithDefault();   

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Default production range cut (mm)   " << defaultRangeCut   << G4endl;
#endif

  G4double prodCutPhotons   = globalConsts->GetProdCutPhotons();
  G4double prodCutElectrons = globalConsts->GetProdCutElectrons();
  G4double prodCutPositrons = globalConsts->GetProdCutPositrons();
  G4double prodCutHadrons   = globalConsts->GetProdCutHadrons();  

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Photon production range cut (mm)   " << prodCutPhotons   << G4endl;
  G4cout << __METHOD_NAME__ << "Electron production range cut (mm) " << prodCutElectrons << G4endl;
  G4cout << __METHOD_NAME__ << "Positron production range cut (mm) " << prodCutPositrons << G4endl;
  G4cout << __METHOD_NAME__ << "Hadron production range cut (mm)   " << prodCutHadrons<< G4endl;
#endif

  // BDSIM's default range cuts (0.7mm) are different from geant4 defaults (1mm) so always set.
  SetCutValue(prodCutPhotons,"gamma");
  SetCutValue(prodCutElectrons,"e-");
  SetCutValue(prodCutPositrons,"e+");
  SetCutValue(prodCutHadrons,"proton");

  // Looping over specific particles?
  //G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  //G4ParticleTable::G4PTblDicIterator* particleIterator = particleTable->GetIterator();
  
  DumpCutValuesTable(); 
}  

void BDSModularPhysicsList::SetParticleDefinition()
{
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;

  // set primary particle definition and kinetic beam parameters other than total energy
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  BDSGlobalConstants::Instance()->SetParticleDefinition(particleTable->
							FindParticle(BDSGlobalConstants::Instance()->GetParticleName()) );  
  
  if(!BDSGlobalConstants::Instance()->GetParticleDefinition()) 
    {
      G4Exception("Particle not found, quitting!", "-1", FatalException, "");
      exit(1);
    }
  
  // set kinetic beam parameters other than total energy
  BDSGlobalConstants::Instance()->SetBeamMomentum(sqrt(pow(BDSGlobalConstants::Instance()->GetBeamTotalEnergy(),2)-
						       pow(BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass(),2)) );
  
  BDSGlobalConstants::Instance()->SetBeamKineticEnergy(BDSGlobalConstants::Instance()->GetBeamTotalEnergy() - 
						       BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass() );
  
  BDSGlobalConstants::Instance()->SetParticleMomentum(sqrt(pow(BDSGlobalConstants::Instance()->GetParticleTotalEnergy(),2)-
							   pow(BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass(),2)) );
  
  BDSGlobalConstants::Instance()->SetParticleKineticEnergy(BDSGlobalConstants::Instance()->GetParticleTotalEnergy() - 
							   BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass() );
  
  G4cout << __METHOD_NAME__ << "Beam properties:"<<G4endl;
  G4cout << __METHOD_NAME__ << "Particle : " 
	 << BDSGlobalConstants::Instance()->GetParticleDefinition()->GetParticleName()<<G4endl;
  G4cout << __METHOD_NAME__ << "Mass : " 
	 << BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass()/CLHEP::GeV<< " GeV"<<G4endl;
  G4cout << __METHOD_NAME__ << "Charge : " 
	 << BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGCharge()<< " e"<<G4endl;
  G4cout << __METHOD_NAME__ << "Total Energy : "
	 << BDSGlobalConstants::Instance()->GetBeamTotalEnergy()/CLHEP::GeV<<" GeV"<<G4endl;
  G4cout << __METHOD_NAME__ << "Kinetic Energy : "
	 << BDSGlobalConstants::Instance()->GetBeamKineticEnergy()/CLHEP::GeV<<" GeV"<<G4endl;
  G4cout << __METHOD_NAME__ << "Momentum : "
	 << BDSGlobalConstants::Instance()->GetBeamMomentum()/CLHEP::GeV<<" GeV"<<G4endl;
}

void BDSModularPhysicsList::LoadEm()
{
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  if(!_emPhysics){
    _emPhysics = new G4EmStandardPhysics();		  
    _constructors.push_back(_emPhysics);			  
    LoadParameterisationPhysics();	
  }		  
}							  
							  
void BDSModularPhysicsList::LoadEmLow()
{
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;			  
  if(!_emPhysics){
    _emPhysics = new G4EmPenelopePhysics();		  
    _constructors.push_back(_emPhysics);			  
    LoadParameterisationPhysics();			  
  }
}							  
							  
void BDSModularPhysicsList::LoadParameterisationPhysics()
{
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  if(!_paramPhysics){
    _paramPhysics = new BDSParameterisationPhysics();	  
    _constructors.push_back(_paramPhysics);		  
  }
}							  
							  
void BDSModularPhysicsList::LoadHadronic()
{
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  if(!_hadronicPhysics){
#if G4VERSION_NUMBER < 1000
    _hadronicPhysics = new HadronPhysicsQGSP_BERT();
#else
    _hadronicPhysics = new G4HadronPhysicsQGSP_BERT();
#endif
    _constructors.push_back(_hadronicPhysics);		  
  }
}							  
							  
void BDSModularPhysicsList::LoadHadronicHP()
{
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  if(!_hadronicPhysics){
#if G4VERSION_NUMBER < 1000
    _hadronicPhysics = new HadronPhysicsQGSP_BERT_HP();
#else
    _hadronicPhysics = new G4HadronPhysicsQGSP_BERT_HP();
#endif
    _constructors.push_back(_hadronicPhysics);		  
  }
}							  
							  
void BDSModularPhysicsList::LoadSynchRad()
{		    
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  if(!_synchRadPhysics){
    _synchRadPhysics = new BDSSynchRadPhysics();		  
    _constructors.push_back(_synchRadPhysics);		  
  }
  // Switch on BDSGlobalConstants::SetSynchRadOn() to keep BDSPhysicsListCompatibility
  BDSGlobalConstants::Instance()->SetSynchRadOn(true);
}							  
							  
void BDSModularPhysicsList::LoadMuon()
{			  
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  if(!_muonPhysics){
    _muonPhysics = new BDSMuonPhysics();			  
    _constructors.push_back(_muonPhysics);		  
  }
}							  
							  
void BDSModularPhysicsList::LoadOptical()
{
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  if(!_opticalPhysics){
    _opticalPhysics = new G4OpticalPhysics();		  
    _constructors.push_back(_opticalPhysics);		  
  }
}							  
							  
void BDSModularPhysicsList::LoadDecay()
{			  
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  if(!_decayPhysics){
    _decayPhysics = new G4DecayPhysics();			  
    _constructors.push_back(_decayPhysics);		  
  }
}                                                         

void BDSModularPhysicsList::LoadCutsAndLimits()
{
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  if(verbose || debug) 
    G4cout << __METHOD_NAME__ << G4endl;
  if(!_cutsAndLimits){
    _cutsAndLimits = new BDSCutsAndLimits();			  
    _constructors.push_back(_cutsAndLimits);		  
  }
}                                                         
