#include "BDSAwakePhysicsList.hh"
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4OpticalPhysics.hh"
#include "G4OpticalProcessIndex.hh"
#include "G4ParticleTable.hh"
#include "BDSSynchRadPhysics.hh"
#include "BDSGlobalConstants.hh"
#include "G4EmStandardPhysics.hh"
#include "BDSParameterisationPhysics.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4DecayPhysics.hh"
#include "BDSSynchRadPhysics.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Proton.hh"
#include "G4AntiProton.hh"

//Note: transportation process is constructed by default with classes derive from G4VModularPhysicsList

BDSAwakePhysicsList::BDSAwakePhysicsList(): G4VModularPhysicsList(),_physListName(BDSGlobalConstants::Instance()->GetPhysListName()) {
  SetVerboseLevel(1);
  _emPhysics=NULL;
  _hadronicPhysics=NULL;
  _muonPhysics=NULL;
  _opticalPhysics=NULL;
  _decayPhysics=NULL;
  _paramPhysics=NULL;
  _synchRadPhysics=NULL;
  
  ParsePhysicsList();
  ConfigurePhysics();
  Register();
  ConstructMinimumParticleSet();
  SetParticleDefinition();
}

void BDSAwakePhysicsList::Print(){
  //  aParticleIterator->reset();
  //  for(vector<G4VPhysicsConstructor*>::iterator it = _constructors.begin();
  //      it != _constructors.end();
  //      ++_constructors){
  //    (*it)->
  //  }
}

//Parse the physicsList option
void BDSAwakePhysicsList::ParsePhysicsList(){
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

void BDSAwakePhysicsList::ConstructMinimumParticleSet(){
  //Minimum required set of particles required for tracking
  G4Electron::Electron();
  G4Positron::Positron();
  G4Proton::Proton();
  G4AntiProton::AntiProton();
}

void BDSAwakePhysicsList::ConfigurePhysics(){
  if(_opticalPhysics){ ConfigureOptical();}
}

void BDSAwakePhysicsList::ConfigureOptical(){
  if (!_opticalPhysics) return;
  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  _opticalPhysics->Configure(kCerenkov,  globals->GetTurnOnCerenkov());///< Cerenkov process index                                   
  _opticalPhysics->Configure(kScintillation, true);///< Scintillation process index                              
  _opticalPhysics->Configure(kAbsorption, globals->GetTurnOnOpticalAbsorption());///< Absorption process index                                 
  _opticalPhysics->Configure(kRayleigh, globals->GetTurnOnRayleighScattering());///< Rayleigh scattering process index                        
  _opticalPhysics->Configure(kMieHG, globals->GetTurnOnMieScattering());///< Mie scattering process index                             
  _opticalPhysics->Configure(kBoundary, globals->GetTurnOnOpticalSurface());///< Boundary process index                                   
  _opticalPhysics->Configure(kWLS, true);///< Wave Length Shifting process index                       
    //    _opticalPhysics->Configure(kNoProcess,      globals->GetTurnOn< Number of processes, no selected process
  _opticalPhysics->SetScintillationYieldFactor(globals->GetScintYieldFactor());
}

void BDSAwakePhysicsList::Register(){
  std::vector<G4VPhysicsConstructor*>::iterator it;
  for(it = _constructors.begin(); it != _constructors.end(); it++){
    RegisterPhysics(*it);
  }
}

BDSAwakePhysicsList::~BDSAwakePhysicsList()
{ 
}


void BDSAwakePhysicsList::SetCuts()
{
  G4VUserPhysicsList::SetCuts();

  SetCutsWithDefault();   
  
  if(BDSGlobalConstants::Instance()->GetProdCutPhotons()>0)
    SetCutValue(BDSGlobalConstants::Instance()->GetProdCutPhotons(),G4ProductionCuts::GetIndex("gamma"));
  
  if(BDSGlobalConstants::Instance()->GetProdCutElectrons()>0)
    SetCutValue(BDSGlobalConstants::Instance()->GetProdCutElectrons(),G4ProductionCuts::GetIndex("e-"));
  
  if(BDSGlobalConstants::Instance()->GetProdCutPositrons()>0)
    SetCutValue(BDSGlobalConstants::Instance()->GetProdCutPositrons(),G4ProductionCuts::GetIndex("e+"));
  
  DumpCutValuesTable(); 
}  

void BDSAwakePhysicsList::SetParticleDefinition(){
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
  BDSGlobalConstants::Instance()->SetBeamMomentum( sqrt(pow(BDSGlobalConstants::Instance()->GetBeamTotalEnergy(),2)-
							pow(BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass(),2)) );
  
  BDSGlobalConstants::Instance()->SetBeamKineticEnergy(BDSGlobalConstants::Instance()->GetBeamTotalEnergy() - 
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

void BDSAwakePhysicsList::LoadEm(){			  
  _emPhysics = new G4EmStandardPhysics();		  
  _constructors.push_back(_emPhysics);			  
  LoadParameterisationPhysics();			  
}							  
							  
void BDSAwakePhysicsList::LoadEmLow(){			  
  _emPhysics = new G4EmStandardPhysics();		  
  _constructors.push_back(_emPhysics);			  
  LoadParameterisationPhysics();			  
}							  
							  
void BDSAwakePhysicsList::LoadParameterisationPhysics(){  
  _paramPhysics = new BDSParameterisationPhysics();	  
  _constructors.push_back(_paramPhysics);		  
}							  
							  
void BDSAwakePhysicsList::LoadHadronic(){		  
  _hadronicPhysics = new G4HadronPhysicsQGSP_BERT();	  
  _constructors.push_back(_hadronicPhysics);		  
}							  
							  
void BDSAwakePhysicsList::LoadHadronicHP(){		  
  _hadronicPhysics = new G4HadronPhysicsQGSP_BERT_HP();	  
  _constructors.push_back(_hadronicPhysics);		  
}							  
							  
void BDSAwakePhysicsList::LoadSynchRad(){		  
  _synchRadPhysics = new BDSSynchRadPhysics();		  
  _constructors.push_back(_synchRadPhysics);		  
}							  
							  
void BDSAwakePhysicsList::LoadMuon(){			  
  _muonPhysics = new BDSMuonPhysics();			  
  _constructors.push_back(_muonPhysics);		  
}							  
							  
void BDSAwakePhysicsList::LoadOptical(){		  
  _opticalPhysics = new G4OpticalPhysics();		  
  _constructors.push_back(_opticalPhysics);		  
}							  
							  
void BDSAwakePhysicsList::LoadDecay(){			  
  _decayPhysics = new G4DecayPhysics();			  
  _constructors.push_back(_decayPhysics);		  
}                                                         

void BDSAwakePhysicsList::LoadCutsAndLimits(){			  
  _cutsAndLimits = new BDSCutsAndLimits();			  
  _constructors.push_back(_cutsAndLimits);		  
}                                                         
