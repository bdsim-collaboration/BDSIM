#include "BDSCutsAndLimits.hh"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"
#include "BDSModularPhysicsList.hh"
#include "BDSMuonPhysics.hh"
#include "BDSParameterisationPhysics.hh"
#include "BDSSynchRadPhysics.hh"

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

#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"

#include "G4HadronPhysicsFTFP_BERT.hh"
#include "G4HadronPhysicsFTFP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BIC.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"

#include <iterator>
#include <map>
#include <ostream>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

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
  opticalPhysics  = nullptr;

  physicsConstructors.insert(std::make_pair("cutsandlimits",&BDSModularPhysicsList::CutsAndLimits));
  physicsConstructors.insert(std::make_pair("em",           &BDSModularPhysicsList::Em));
  physicsConstructors.insert(std::make_pair("em_low",       &BDSModularPhysicsList::EmLow));
  physicsConstructors.insert(std::make_pair("hadronic",     &BDSModularPhysicsList::QGSPBERT));
  physicsConstructors.insert(std::make_pair("hadronichp",   &BDSModularPhysicsList::QGSPBERTHP));
  physicsConstructors.insert(std::make_pair("synchrad",     &BDSModularPhysicsList::SynchRad));
  physicsConstructors.insert(std::make_pair("muon",         &BDSModularPhysicsList::Muon));
  physicsConstructors.insert(std::make_pair("optical",      &BDSModularPhysicsList::Optical));
  physicsConstructors.insert(std::make_pair("decay",        &BDSModularPhysicsList::Decay));
  physicsConstructors.insert(std::make_pair("qgsp_bert",    &BDSModularPhysicsList::QGSPBERT));
  physicsConstructors.insert(std::make_pair("qgsp_bert_hp", &BDSModularPhysicsList::QGSPBERTHP));
  physicsConstructors.insert(std::make_pair("qgsp_bic",     &BDSModularPhysicsList::QGSPBIC));
  physicsConstructors.insert(std::make_pair("qgsp_bic_hp",  &BDSModularPhysicsList::QGSPBICHP));
  physicsConstructors.insert(std::make_pair("ftfp_bert",    &BDSModularPhysicsList::FTFPBERT));
  physicsConstructors.insert(std::make_pair("ftfp_bert_hp", &BDSModularPhysicsList::FTFPBERTHP));

  // prepare vector of valid names for searching when parsing physics list string
  for (const auto& constructor : physicsConstructors)
    {
      physicsLists.push_back(constructor.first);
      physicsActivated[constructor.first] = false;
    }
  
  ParsePhysicsList();
  ConfigurePhysics();
  Register();
  ConstructMinimumParticleSet();
  SetParticleDefinition();
  SetCuts();
  DumpCutValuesTable(100);

#ifdef BDSDEBUG
  Print();
  
  auto particleName = BDSGlobalConstants::Instance()->GetParticleName();
  G4cout << "Register physics processes by name for the primary particle \"" << particleName << "\":" << G4endl;
  
  auto pl = G4ParticleTable::GetParticleTable()->FindParticle(particleName)->GetProcessManager()->GetProcessList();
  for (G4int i = 0; i < pl->length(); i++)
    {G4cout << "\"" << (*pl)[i]->GetProcessName() << "\"" << G4endl;}
#endif
}

BDSModularPhysicsList::~BDSModularPhysicsList()
{;}

void BDSModularPhysicsList::Print()
{
  for (const auto& physics : physicsActivated)
    {
      G4String result = (physics.second ? "activated" : "inactive");
      G4cout << "\"" << physics.first << "\" : " << result << G4endl;
    }
}

//Parse the physicsList option
void BDSModularPhysicsList::ParsePhysicsList()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "physics list string: \"" << physListName << "\"" << G4endl;
#endif
  std::stringstream ss(physListName);
  std::istream_iterator<std::string> begin(ss);
  std::istream_iterator<std::string> end;
  std::vector<std::string> vstrings(begin, end);

  for (auto name : vstrings)
    {
      G4String nameLower(name);
      nameLower.toLower();
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "Constructing \"" << nameLower << "\"" << G4endl;
#endif
      auto result = physicsConstructors.find(nameLower);
      if (result != physicsConstructors.end())
	{
	  auto mem = result->second;
	  (this->*mem)(); // call the function pointer in this instance of the class
	}
      else
	{
	  G4cout << "\"" << nameLower << "\" is not a valid physics list. Available ones are: " << G4endl;
	  for (auto name : physicsLists)
	    {G4cout << "\"" << name << "\"" << G4endl;}
	  exit(1);
	}
    }

  //Always load cuts and limits.
  CutsAndLimits();
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
    {G4cout << __METHOD_NAME__ << G4endl;}
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
    {G4cout << __METHOD_NAME__ << G4endl;}
  for(auto physics : constructors)
    {RegisterPhysics(physics);}
}

void BDSModularPhysicsList::SetCuts()
{
  if(verbose || debug) 
    {G4cout << __METHOD_NAME__ << G4endl;}

  G4VUserPhysicsList::SetCuts();  
  G4double defaultRangeCut  = globals->GetDefaultRangeCut(); 
  SetDefaultCutValue(defaultRangeCut);
  SetCutsWithDefault();

  G4double prodCutPhotons   = globals->GetProdCutPhotons();
  G4double prodCutElectrons = globals->GetProdCutElectrons();
  G4double prodCutPositrons = globals->GetProdCutPositrons();
  G4double prodCutProtons   = globals->GetProdCutProtons();

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Default production range cut  " << defaultRangeCut  << " mm" << G4endl;
  G4cout << __METHOD_NAME__ << "Photon production range cut   " << prodCutPhotons   << " mm" << G4endl;
  G4cout << __METHOD_NAME__ << "Electron production range cut " << prodCutElectrons << " mm" << G4endl;
  G4cout << __METHOD_NAME__ << "Positron production range cut " << prodCutPositrons << " mm" << G4endl;
  G4cout << __METHOD_NAME__ << "Proton production range cut   " << prodCutProtons   << " mm" << G4endl;
#endif
  
  SetCutValue(prodCutPhotons,  "gamma");
  SetCutValue(prodCutElectrons,"e-");
  SetCutValue(prodCutPositrons,"e+");
  SetCutValue(prodCutProtons,  "proton");

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "List of all constructed particles by physics lists" << G4endl;
  for (auto particle : *G4ParticleTable::fDictionary)
    {G4cout << particle.second->GetParticleName() << ", ";}
  G4cout << G4endl;
#endif
  
  DumpCutValuesTable(); 
}  

void BDSModularPhysicsList::SetParticleDefinition()
{
  if(verbose || debug) 
    {G4cout << __METHOD_NAME__ << G4endl;}

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

void BDSModularPhysicsList::Em()
{
  if(verbose || debug) 
    {G4cout << __METHOD_NAME__ << G4endl;}
  if (!physicsActivated["em"])
    {
      constructors.push_back(new G4EmStandardPhysics());
      physicsActivated["em"] = true;
    }
  ParameterisationPhysics(); // requires parameterisation physics
}							  
							  
void BDSModularPhysicsList::EmLow()
{
  if(verbose || debug)
    {G4cout << __METHOD_NAME__ << G4endl;}
  if (!physicsActivated["em_low"])
    {
      constructors.push_back(new G4EmPenelopePhysics());
      physicsActivated["em_low"] = true;
    }
  ParameterisationPhysics(); // requires parameterisation physics
}							  
							  
void BDSModularPhysicsList::ParameterisationPhysics()
{
  if(verbose || debug) 
    {G4cout << __METHOD_NAME__ << G4endl;}
  if (!physicsActivated["parameterisation"])
    {
      constructors.push_back(new BDSParameterisationPhysics());
      physicsActivated["parameterisation"] = true;
    }
}							  
							  
void BDSModularPhysicsList::SynchRad()
{		    
  if(verbose || debug) 
    {G4cout << __METHOD_NAME__ << G4endl;}
  if(!physicsActivated["synchrad"])
    {
      constructors.push_back(new BDSSynchRadPhysics());
      physicsActivated["synchrad"] = true;
    }
  // Switch on BDSGlobalConstants::SetSynchRadOn() to keep BDSPhysicsListCompatibility
  globals->SetSynchRadOn(true);
}							  
							  
void BDSModularPhysicsList::Muon()
{
  if(verbose || debug) 
    {G4cout << __METHOD_NAME__ << G4endl;}
  if(!physicsActivated["muon"])
    {
      constructors.push_back(new BDSMuonPhysics());
      physicsActivated["muon"] = true;
    }
}							  
							  
void BDSModularPhysicsList::Optical()
{
  if(verbose || debug) 
    {G4cout << __METHOD_NAME__ << G4endl;}
  if(!physicsActivated["optical"])
    {
      opticalPhysics = new G4OpticalPhysics();		  
      constructors.push_back(opticalPhysics);
      physicsActivated["optical"] = true;
    }
}							  
							  
void BDSModularPhysicsList::Decay()
{
  if(verbose || debug) 
    {G4cout << __METHOD_NAME__ << G4endl;}
  if(!physicsActivated["decay"])
    {
      constructors.push_back(new G4DecayPhysics());
      physicsActivated["decay"] = true;
    }
}                                                         

void BDSModularPhysicsList::CutsAndLimits()
{
  if(verbose || debug) 
    {G4cout << __METHOD_NAME__ << G4endl;}
  if(!physicsActivated["cutsandlimits"])
    {
      constructors.push_back(new BDSCutsAndLimits());
      physicsActivated["cutsandlimits"] = true;
    }
}           

void BDSModularPhysicsList::QGSPBERT()
{
  if(verbose || debug) 
    {G4cout << __METHOD_NAME__ << G4endl;}
  if(!physicsActivated["qgsp_bert"])
    {
      constructors.push_back(new G4HadronPhysicsQGSP_BERT());
      physicsActivated["qgsp_bert"] = true;
    }
}

void BDSModularPhysicsList::QGSPBERTHP()
{
  if(verbose || debug) 
    {G4cout << __METHOD_NAME__ << G4endl;}
  if(!physicsActivated["qgsp_bert_hp"])
    {
      constructors.push_back(new G4HadronPhysicsQGSP_BERT_HP());
      physicsActivated["qgsp_bert_hp"] = true;
    }
}

void BDSModularPhysicsList::QGSPBIC()
{
  if(verbose || debug) 
    {G4cout << __METHOD_NAME__ << G4endl;}
  if(!physicsActivated["qgsp_bic"])
    {
      constructors.push_back(new G4HadronPhysicsQGSP_BIC());
      physicsActivated["qgsp_bic"] = true;
    }
}

void BDSModularPhysicsList::QGSPBICHP()
{
  if(verbose || debug) 
    {G4cout << __METHOD_NAME__ << G4endl;}
  if(!physicsActivated["qgsp_bic_hp"])
    {
      constructors.push_back(new G4HadronPhysicsQGSP_BIC_HP());
      physicsActivated["qgsp_bic_hp"] = true;
    }
}

void BDSModularPhysicsList::FTFPBERT()
{
  if(verbose || debug) 
    {G4cout << __METHOD_NAME__ << G4endl;}
  if(!physicsActivated["ftfp_bert"])
    {
      constructors.push_back(new G4HadronPhysicsFTFP_BERT());
      physicsActivated["ftfp_bert"] = true;
    }
}

void BDSModularPhysicsList::FTFPBERTHP()
{
  if(verbose || debug) 
    {G4cout << __METHOD_NAME__ << G4endl;}
  if(!physicsActivated["ftfp_bert_hp"])
    {
      constructors.push_back(new G4HadronPhysicsFTFP_BERT_HP());
      physicsActivated["ftfp_bert_hp"] = true;
    }
}
