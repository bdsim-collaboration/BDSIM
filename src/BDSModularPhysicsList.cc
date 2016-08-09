#include "BDSCutsAndLimits.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSModularPhysicsList.hh"
#include "BDSMuonPhysics.hh"
#include "BDSParameterisationPhysics.hh"
#include "BDSSynchRadPhysics.hh"

// physics processes / builders
#include "G4DecayPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronPhysicsFTFP_BERT.hh"
#include "G4HadronPhysicsFTFP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BIC.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4OpticalPhysics.hh"
#include "G4SynchrotronRadiation.hh"

// particles
#include "G4AntiNeutrinoE.hh"
#include "G4AntiNeutron.hh"
#include "G4AntiProton.hh"
#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4LeptonConstructor.hh"
#include "G4NeutrinoE.hh"
#include "G4Neutron.hh"
#include "G4Positron.hh"
#include "G4Proton.hh"
#include "G4ShortLivedConstructor.hh"


// general geant4
#include "globals.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"

#include <iterator>
#include <map>
#include <ostream>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

//#include "G4MesonConstructor.hh"

BDSModularPhysicsList::BDSModularPhysicsList(G4String physicsList):
  opticalPhysics(nullptr)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  
  verbose = BDSGlobalConstants::Instance()->Verbose();
  globals = BDSGlobalConstants::Instance();
  
  SetVerboseLevel(1);

  physicsConstructors.insert(std::make_pair("cutsandlimits",    &BDSModularPhysicsList::CutsAndLimits));
  physicsConstructors.insert(std::make_pair("em",               &BDSModularPhysicsList::Em));
  physicsConstructors.insert(std::make_pair("em_extra",         &BDSModularPhysicsList::EmExtra));
  physicsConstructors.insert(std::make_pair("em_low",           &BDSModularPhysicsList::EmLow));
  physicsConstructors.insert(std::make_pair("hadronic_elastic", &BDSModularPhysicsList::HadronicElastic));
  physicsConstructors.insert(std::make_pair("hadronic",         &BDSModularPhysicsList::QGSPBERT));
  physicsConstructors.insert(std::make_pair("hadronic_hp",      &BDSModularPhysicsList::QGSPBERTHP));
  physicsConstructors.insert(std::make_pair("synchrad",         &BDSModularPhysicsList::SynchRad));
  physicsConstructors.insert(std::make_pair("parameterisation", &BDSModularPhysicsList::ParameterisationPhysics));
  physicsConstructors.insert(std::make_pair("muon",             &BDSModularPhysicsList::Muon));
  physicsConstructors.insert(std::make_pair("optical",          &BDSModularPhysicsList::Optical));
  physicsConstructors.insert(std::make_pair("decay",            &BDSModularPhysicsList::Decay));
  physicsConstructors.insert(std::make_pair("qgsp_bert",        &BDSModularPhysicsList::QGSPBERT));
  physicsConstructors.insert(std::make_pair("qgsp_bert_hp",     &BDSModularPhysicsList::QGSPBERTHP));
  physicsConstructors.insert(std::make_pair("qgsp_bic",         &BDSModularPhysicsList::QGSPBIC));
  physicsConstructors.insert(std::make_pair("qgsp_bic_hp",      &BDSModularPhysicsList::QGSPBICHP));
  physicsConstructors.insert(std::make_pair("ftfp_bert",        &BDSModularPhysicsList::FTFPBERT));
  physicsConstructors.insert(std::make_pair("ftfp_bert_hp",     &BDSModularPhysicsList::FTFPBERTHP));

  // prepare vector of valid names for searching when parsing physics list string
  for (const auto& constructor : physicsConstructors)
    {
      physicsLists.push_back(constructor.first);
      physicsActivated[constructor.first] = false;
    }
  
  ParsePhysicsList(physicsList);
  ConfigurePhysics();

  // register the physics constructors with base class mechanics.
  for(auto physics : constructors)
    {RegisterPhysics(physics);}
  
  ConstructMinimumParticleSet();
  SetParticleDefinition();
  SetCuts();
  DumpCutValuesTable(100);

#ifdef BDSDEBUG
  Print();
#endif
}

BDSModularPhysicsList::~BDSModularPhysicsList()
{;}

void BDSModularPhysicsList::ConstructParticle()
{
  // mesons
  //G4MesonConstructor mConstructor;
  //mConstructor.ConstructParticle();
  
  // baryons
  //G4BaryonConstructor bConstructor;
  //bConstructor.ConstructParticle();
  
  // ions
  //G4IonConstructor iConstructor;
  //iConstructor.ConstructParticle();

  G4VModularPhysicsList::ConstructParticle();
}

void BDSModularPhysicsList::Print()
{
  for (const auto& physics : physicsActivated)
    {
      G4String result = (physics.second ? "activated" : "inactive");
      G4cout << "\"" << physics.first << "\" : " << result << G4endl;
    }
}

void BDSModularPhysicsList::PrintDefinedParticles() const
{
  G4cout << __METHOD_NAME__ << "Defined particles: " << G4endl;
  auto it = G4ParticleTable::GetParticleTable()->GetIterator();
  it->reset();
  while ((*it)())
  {G4cout <<  it->value()->GetParticleName() << " ";}
  G4cout << G4endl;
}

void BDSModularPhysicsList::PrintPrimaryParticleProcesses() const
{
  auto particleName = globals->ParticleName();
  G4cout << "Register physics processes by name for the primary particle \"" << particleName << "\":" << G4endl;
  
  auto pl = G4ParticleTable::GetParticleTable()->FindParticle(particleName)->GetProcessManager()->GetProcessList();
  for (G4int i = 0; i < pl->length(); i++)
    {G4cout << "\"" << (*pl)[i]->GetProcessName() << "\"" << G4endl;}
}

void BDSModularPhysicsList::ParsePhysicsList(G4String physListName)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Physics list string: \"" << physListName << "\"" << G4endl;
#endif
  std::stringstream ss(physListName);
  std::istream_iterator<std::string> begin(ss);
  std::istream_iterator<std::string> end;
  std::vector<std::string> vstrings(begin, end);

  for (auto name : vstrings)
    {
      G4String nameLower(name);
      nameLower.toLower(); // case insensitive
      auto result = physicsConstructors.find(nameLower);
      if (result != physicsConstructors.end())
	{
	  G4cout << __METHOD_NAME__ << "Constructing \"" << result->first << "\"" << G4endl;
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
    {G4cout << __METHOD_NAME__ << G4endl;}

  // e-, e+, v_e, v_e(bar)
  G4Electron::ElectronDefinition();
  G4Positron::PositronDefinition();
  G4NeutrinoE::NeutrinoEDefinition();
  G4AntiNeutrinoE::AntiNeutrinoEDefinition();

  // p, pbar, neutron, anti-neutron
  G4Proton::ProtonDefinition();
  G4AntiProton::AntiProtonDefinition();
  G4Neutron::NeutronDefinition();
  G4AntiNeutron::AntiNeutronDefinition();

  // photon
  G4Gamma::Gamma();
}

void BDSModularPhysicsList::ConstructAllLeptons()
{
  G4LeptonConstructor leptons;
  leptons.ConstructParticle();
}

void BDSModularPhysicsList::ConstructAllShortLived()
{
  G4ShortLivedConstructor pShortLivedConstructor;
  pShortLivedConstructor.ConstructParticle();
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
    {G4cout << __METHOD_NAME__ << G4endl;}
  if (!opticalPhysics)
    {return;}
  opticalPhysics->Configure(kCerenkov, globals->TurnOnCerenkov());           ///< Cerenkov process index
  opticalPhysics->Configure(kScintillation, true);                                   ///< Scintillation process index                              
  opticalPhysics->Configure(kAbsorption, globals->TurnOnOpticalAbsorption());  ///< Absorption process index
  opticalPhysics->Configure(kRayleigh, globals->TurnOnRayleighScattering()); ///< Rayleigh scattering process index
  opticalPhysics->Configure(kMieHG, globals->TurnOnMieScattering());      ///< Mie scattering process index
  opticalPhysics->Configure(kBoundary, globals->TurnOnOpticalSurface());     ///< Boundary process index
  opticalPhysics->Configure(kWLS,           true);                                    ///< Wave Length Shifting process index                       
// opticalPhysics->Configure(kNoProcess,      globals->GetTurnOn< Number of processes, no selected process
  opticalPhysics->SetScintillationYieldFactor(globals->ScintYieldFactor());
}

void BDSModularPhysicsList::SetCuts()
{
  if(verbose || debug) 
    {G4cout << __METHOD_NAME__ << G4endl;}

  G4VUserPhysicsList::SetCuts();  
  G4double defaultRangeCut  = globals->DefaultRangeCut();
  SetDefaultCutValue(defaultRangeCut);
  SetCutsWithDefault();

  G4double prodCutPhotons   = globals->ProdCutPhotons();
  G4double prodCutElectrons = globals->ProdCutElectrons();
  G4double prodCutPositrons = globals->ProdCutPositrons();
  G4double prodCutProtons   = globals->ProdCutProtons();

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
  globals->SetParticleDefinition(particleTable->FindParticle(globals->ParticleName()));
  
  if(!globals->GetParticleDefinition()) 
    {G4Exception("Particle not found, quitting!", "-1", FatalException, ""); exit(1);}
  
  // set kinetic beam parameters other than total energy
  globals->SetBeamMomentum(sqrt(pow(globals->BeamTotalEnergy(),2)-pow(globals->GetParticleDefinition()->GetPDGMass(),2)));
  globals->SetBeamKineticEnergy(globals->BeamTotalEnergy()-globals->GetParticleDefinition()->GetPDGMass());
  globals->SetParticleMomentum(sqrt(pow(globals->ParticleTotalEnergy(),2)-pow(globals->GetParticleDefinition()->GetPDGMass(),2)));
  globals->SetParticleKineticEnergy(globals->ParticleTotalEnergy()-globals->GetParticleDefinition()->GetPDGMass());
  
  G4cout << __METHOD_NAME__ << "Beam properties:"<<G4endl;
  G4cout << __METHOD_NAME__ << "Particle : " 
	 << globals->GetParticleDefinition()->GetParticleName()<<G4endl;
  G4cout << __METHOD_NAME__ << "Mass : " 
	 << globals->GetParticleDefinition()->GetPDGMass()/CLHEP::GeV<< " GeV"<<G4endl;
  G4cout << __METHOD_NAME__ << "Charge : " 
	 << globals->GetParticleDefinition()->GetPDGCharge()<< " e"<<G4endl;
  G4cout << __METHOD_NAME__ << "Total Energy : "
	 << globals->BeamTotalEnergy()/CLHEP::GeV<<" GeV"<<G4endl;
  G4cout << __METHOD_NAME__ << "Kinetic Energy : "
	 << globals->BeamKineticEnergy()/CLHEP::GeV<<" GeV"<<G4endl;
  G4cout << __METHOD_NAME__ << "Momentum : "
	 << globals->BeamMomentum()/CLHEP::GeV<<" GeV"<<G4endl;
}

void BDSModularPhysicsList::Em()
{
  ConstructAllLeptons();
  if (!physicsActivated["em"])
    {
      constructors.push_back(new G4EmStandardPhysics());
      physicsActivated["em"] = true;
    }
  ParameterisationPhysics(); // requires parameterisation physics
}

void BDSModularPhysicsList::EmExtra()
{
  ConstructAllLeptons();
  ConstructAllShortLived();
  //  Construct resonances and quarks
  G4ShortLivedConstructor pShortLivedConstructor;
  pShortLivedConstructor.ConstructParticle();
  if (!physicsActivated["em_extra"])
    {
      auto constructor = new G4EmExtraPhysics();
      constructor->Synch(true);
      constructors.push_back(constructor);
      physicsActivated["em_extra"] = true;
    }
  ParameterisationPhysics(); // requires parameterisation physics
}
							  
void BDSModularPhysicsList::EmLow()
{
  ConstructAllLeptons();
  if (!physicsActivated["em_low"])
    {
      constructors.push_back(new G4EmPenelopePhysics());
      physicsActivated["em_low"] = true;
    }
  ParameterisationPhysics(); // requires parameterisation physics
}

void BDSModularPhysicsList::HadronicElastic()
{
  ConstructAllLeptons();
  if (!physicsActivated["hadronic_elastic"])
    {
      constructors.push_back(new G4HadronElasticPhysics());
      physicsActivated["hadronic_elastic"] = true;
    }
}
							  
void BDSModularPhysicsList::ParameterisationPhysics()
{
  if (!physicsActivated["parameterisation"])
    {
      constructors.push_back(new BDSParameterisationPhysics());
      physicsActivated["parameterisation"] = true;
    }
}							  
							  
void BDSModularPhysicsList::SynchRad()
{
  ConstructAllLeptons();
  if(!physicsActivated["synchrad"])
    {
      constructors.push_back(new BDSSynchRadPhysics());
      physicsActivated["synchrad"] = true;
    }
}							  
							  
void BDSModularPhysicsList::Muon()
{
  if(!physicsActivated["muon"])
    {
      constructors.push_back(new BDSMuonPhysics());
      physicsActivated["muon"] = true;
    }
}							  
							  
void BDSModularPhysicsList::Optical()
{
  if(!physicsActivated["optical"])
    {
      opticalPhysics = new G4OpticalPhysics();		  
      constructors.push_back(opticalPhysics);
      physicsActivated["optical"] = true;
    }
}							  
							  
void BDSModularPhysicsList::Decay()
{
  if(!physicsActivated["decay"])
    {
      constructors.push_back(new G4DecayPhysics());
      physicsActivated["decay"] = true;
    }
}                                                         

void BDSModularPhysicsList::CutsAndLimits()
{
  if(!physicsActivated["cutsandlimits"])
    {
      constructors.push_back(new BDSCutsAndLimits());
      physicsActivated["cutsandlimits"] = true;
    }
}           

void BDSModularPhysicsList::QGSPBERT()
{
  ConstructAllLeptons();
  if(!physicsActivated["qgsp_bert"])
    {
      constructors.push_back(new G4HadronPhysicsQGSP_BERT());
      physicsActivated["qgsp_bert"] = true;
    }
}

void BDSModularPhysicsList::QGSPBERTHP()
{
  ConstructAllLeptons();
  if(!physicsActivated["qgsp_bert_hp"])
    {
      constructors.push_back(new G4HadronPhysicsQGSP_BERT_HP());
      physicsActivated["qgsp_bert_hp"] = true;
    }
}

void BDSModularPhysicsList::QGSPBIC()
{
  ConstructAllLeptons();
  if(!physicsActivated["qgsp_bic"])
    {
      constructors.push_back(new G4HadronPhysicsQGSP_BIC());
      physicsActivated["qgsp_bic"] = true;
    }
}

void BDSModularPhysicsList::QGSPBICHP()
{
  ConstructAllLeptons();
  if(!physicsActivated["qgsp_bic_hp"])
    {
      constructors.push_back(new G4HadronPhysicsQGSP_BIC_HP());
      physicsActivated["qgsp_bic_hp"] = true;
    }
}

void BDSModularPhysicsList::FTFPBERT()
{
  ConstructAllLeptons();
  HadronicElastic();
  if(!physicsActivated["ftfp_bert"])
    {
      constructors.push_back(new G4HadronPhysicsFTFP_BERT());
      physicsActivated["ftfp_bert"] = true;
    }
}

void BDSModularPhysicsList::FTFPBERTHP()
{
  ConstructAllLeptons();
  HadronicElastic();
  if(!physicsActivated["ftfp_bert_hp"])
    {
      constructors.push_back(new G4HadronPhysicsFTFP_BERT_HP());
      physicsActivated["ftfp_bert_hp"] = true;
    }
}
