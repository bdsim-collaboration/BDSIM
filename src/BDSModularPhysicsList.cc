#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSModularPhysicsList.hh"
#include "BDSPhysicsCherenkov.hh"
#include "BDSPhysicsCutsAndLimits.hh"
#include "BDSPhysicsLaserWire.hh"
#include "BDSPhysicsMuon.hh"
#include "BDSPhysicsSynchRad.hh"
#include "BDSUtilities.hh"

#include "parser/fastlist.h"
#include "parser/physicsbiasing.h"

// general geant4
#include "globals.hh"
#include "G4GenericBiasingPhysics.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4Version.hh"

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
#include "G4OpticalProcessIndex.hh"
#if G4VERSION_NUMBER > 1020
#include "G4SpinDecayPhysics.hh"
#endif
#include "G4SynchrotronRadiation.hh"

// particles
#include "G4AntiNeutrinoE.hh"
#include "G4AntiNeutron.hh"
#include "G4AntiProton.hh"
#include "G4BaryonConstructor.hh"
#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4IonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4NeutrinoE.hh"
#include "G4Neutron.hh"
#include "G4Positron.hh"
#include "G4Proton.hh"
#include "G4ShortLivedConstructor.hh"

#include <iterator>
#include <map>
#include <ostream>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

BDSModularPhysicsList::BDSModularPhysicsList(G4String physicsList):
  opticalPhysics(nullptr),
  emWillBeUsed(false)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  
  globals = BDSGlobalConstants::Instance();
  
  SetVerboseLevel(1);

  physicsConstructors.insert(std::make_pair("cerenkov",         &BDSModularPhysicsList::Cherenkov));
  physicsConstructors.insert(std::make_pair("cherenkov",        &BDSModularPhysicsList::Cherenkov));
  physicsConstructors.insert(std::make_pair("cutsandlimits",    &BDSModularPhysicsList::CutsAndLimits));
  physicsConstructors.insert(std::make_pair("em",               &BDSModularPhysicsList::Em));
  physicsConstructors.insert(std::make_pair("em_extra",         &BDSModularPhysicsList::EmExtra));
  physicsConstructors.insert(std::make_pair("em_low",           &BDSModularPhysicsList::EmLow));
  physicsConstructors.insert(std::make_pair("hadronic_elastic", &BDSModularPhysicsList::HadronicElastic));
  physicsConstructors.insert(std::make_pair("hadronic",         &BDSModularPhysicsList::QGSPBERT));
  physicsConstructors.insert(std::make_pair("hadronic_hp",      &BDSModularPhysicsList::QGSPBERTHP));
  physicsConstructors.insert(std::make_pair("synchrad",         &BDSModularPhysicsList::SynchRad));
  physicsConstructors.insert(std::make_pair("muon",             &BDSModularPhysicsList::Muon));
  physicsConstructors.insert(std::make_pair("optical",          &BDSModularPhysicsList::Optical));
  physicsConstructors.insert(std::make_pair("decay",            &BDSModularPhysicsList::Decay));
  physicsConstructors.insert(std::make_pair("spindecay",        &BDSModularPhysicsList::SpinDecay));
  physicsConstructors.insert(std::make_pair("qgsp_bert",        &BDSModularPhysicsList::QGSPBERT));
  physicsConstructors.insert(std::make_pair("qgsp_bert_hp",     &BDSModularPhysicsList::QGSPBERTHP));
  physicsConstructors.insert(std::make_pair("qgsp_bic",         &BDSModularPhysicsList::QGSPBIC));
  physicsConstructors.insert(std::make_pair("qgsp_bic_hp",      &BDSModularPhysicsList::QGSPBICHP));
  physicsConstructors.insert(std::make_pair("ftfp_bert",        &BDSModularPhysicsList::FTFPBERT));
  physicsConstructors.insert(std::make_pair("ftfp_bert_hp",     &BDSModularPhysicsList::FTFPBERTHP));
  physicsConstructors.insert(std::make_pair("lw",               &BDSModularPhysicsList::LaserWire));

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
  
#ifdef BDSDEBUG
  Print();
#endif
}

BDSModularPhysicsList::~BDSModularPhysicsList()
{;}

void BDSModularPhysicsList::ConstructParticle()
{
  ConstructMinimumParticleSet();
  G4VModularPhysicsList::ConstructParticle();
  SetParticleDefinition();
}

void BDSModularPhysicsList::ConstructProcess()
{
  G4VModularPhysicsList::ConstructProcess();
  SetCuts();
  DumpCutValuesTable(100);

}

void BDSModularPhysicsList::Print()
{
  for (const auto& physics : physicsActivated)
    {
      G4String result = (physics.second ? "activated" : "inactive");
      G4cout << std::setw(25) << ("\"" + physics.first + "\" : ") << result << G4endl;
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
  // string stream to vector will take a single string that contains words
  // delimited by whitespace and split them on the whitespace
  std::stringstream ss(physListName);
  std::istream_iterator<std::string> begin(ss);
  std::istream_iterator<std::string> end;
  std::vector<std::string> physicsListNamesS(begin, end);

  // convert to G4String for lower case convenience
  std::vector<G4String> physicsListNames;
  for (auto physicsListName : physicsListNamesS)
    {
      G4String name = G4String(physicsListName); // convert string to G4String.
      name.toLower(); // change to lower case - physics lists are case insensitive
      physicsListNames.push_back(name);
    }

  // seach for em physics (could be any order) - needed for different construction of muon phyiscs
  if (std::find(physicsListNames.begin(), physicsListNames.end(), "em") != physicsListNames.end())
    {emWillBeUsed = true;}

  for (const auto name : physicsListNames)
    {
      auto result = physicsConstructors.find(name);
      if (result != physicsConstructors.end())
	{
	  G4cout << __METHOD_NAME__ << "Constructing \"" << result->first << "\" physics list" << G4endl;
	  auto mem = result->second;
	  (this->*mem)(); // call the function pointer in this instance of the class
	}
      else
	{
	  G4cout << "\"" << name << "\" is not a valid physics list. Available ones are: " << G4endl;
	  for (auto listName : physicsLists)
	    {G4cout << "\"" << listName << "\"" << G4endl;}
	  exit(1);
	}
    }

  //Always load cuts and limits.
  CutsAndLimits();
}

void BDSModularPhysicsList::ConstructMinimumParticleSet()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
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

void BDSModularPhysicsList::ConstructAllMesons()
{
  G4MesonConstructor mConstructor;
  mConstructor.ConstructParticle();
}

void BDSModularPhysicsList::ConstructAllBaryons()
{
  G4BaryonConstructor bConstructor;
  bConstructor.ConstructParticle();
}

void BDSModularPhysicsList::ConstructAllIons()
{
  G4IonConstructor iConstructor;
  iConstructor.ConstructParticle();
}

void BDSModularPhysicsList::ConfigurePhysics()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  if(opticalPhysics)
    {ConfigureOptical();}
}

void BDSModularPhysicsList::ConfigureOptical()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  // cherenkov turned on with optical even if it's not on as separate list
  opticalPhysics->Configure(G4OpticalProcessIndex::kCerenkov, true);
  opticalPhysics->Configure(G4OpticalProcessIndex::kScintillation, true);                                ///< Scintillation process index
  opticalPhysics->Configure(G4OpticalProcessIndex::kAbsorption,    globals->TurnOnOpticalAbsorption());  ///< Absorption process index
  opticalPhysics->Configure(G4OpticalProcessIndex::kRayleigh,      globals->TurnOnRayleighScattering()); ///< Rayleigh scattering process index
  opticalPhysics->Configure(G4OpticalProcessIndex::kMieHG,         globals->TurnOnMieScattering());      ///< Mie scattering process index
  opticalPhysics->Configure(G4OpticalProcessIndex::kBoundary,      globals->TurnOnOpticalSurface());     ///< Boundary process index
  opticalPhysics->Configure(G4OpticalProcessIndex::kWLS,           true);                                ///< Wave Length Shifting process index
  opticalPhysics->SetScintillationYieldFactor(globals->ScintYieldFactor());
  G4long maxPhotonsPerStep = globals->MaximumPhotonsPerStep();
  if (maxPhotonsPerStep >= 0)
    {opticalPhysics->SetMaxNumPhotonsPerStep(maxPhotonsPerStep);}
}

void BDSModularPhysicsList::SetCuts()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  
  // set default value
  SetDefaultCutValue(globals->DefaultRangeCut());

  // overwrite when explicitly set in options
  if (globals->ProdCutPhotonsSet())
    {SetCutValue(globals->ProdCutPhotons(),  "gamma");}
  if (globals->ProdCutElectronsSet())
    {SetCutValue(globals->ProdCutElectrons(),"e-");}
  if (globals->ProdCutPositronsSet())
    {SetCutValue(globals->ProdCutPositrons(),"e+");}
  if (globals->ProdCutProtonsSet())
    {SetCutValue(globals->ProdCutProtons(),  "proton");}

  G4cout << __METHOD_NAME__ << "Default production range cut  " << GetDefaultCutValue()  << " mm" << G4endl;
  G4cout << __METHOD_NAME__ << "Photon production range cut   " << GetCutValue("gamma")  << " mm" << G4endl;
  G4cout << __METHOD_NAME__ << "Electron production range cut " << GetCutValue("e-")     << " mm" << G4endl;
  G4cout << __METHOD_NAME__ << "Positron production range cut " << GetCutValue("e+")     << " mm" << G4endl;
  G4cout << __METHOD_NAME__ << "Proton production range cut   " << GetCutValue("proton") << " mm" << G4endl;

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
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  // set primary particle definition and kinetic beam parameters other than total energy
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName = globals->ParticleName();
  globals->SetParticleDefinition(particleTable->FindParticle(particleName));
  
  if(!globals->GetParticleDefinition())
    {
      G4cerr << "Particle \"" << particleName << "\"not found: quitting!" << G4endl;
      exit(1);
    }
  
  // set kinetic beam parameters other than total energy
  globals->SetBeamMomentum(std::sqrt(std::pow(globals->BeamTotalEnergy(),2)-std::pow(globals->GetParticleDefinition()->GetPDGMass(),2)));
  globals->SetBeamKineticEnergy(globals->BeamTotalEnergy()-globals->GetParticleDefinition()->GetPDGMass());
  globals->SetParticleMomentum(std::sqrt(std::pow(globals->ParticleTotalEnergy(),2)-std::pow(globals->GetParticleDefinition()->GetPDGMass(),2)));
  globals->SetParticleKineticEnergy(globals->ParticleTotalEnergy()-globals->GetParticleDefinition()->GetPDGMass());

  // compute signed magnetic rigidity brho
  // formula: B(Tesla)*rho(m) = p(GeV)/(0.299792458 * charge(e))
  // charge (in e units)
  // rigidity (in T*m)
  G4double charge = globals->GetParticleDefinition()->GetPDGCharge();
  G4double brho   = DBL_MAX; // if zero charge infinite magnetic rigidity
  if (BDS::IsFinite(charge)) {
    brho = globals->FFact() * globals->BeamMomentum() / CLHEP::GeV / globals->COverGeV() / charge;
    // rigidity (in Geant4 units)
    brho *= CLHEP::tesla*CLHEP::m;
  }
  // set in globals
  globals->SetBRho(brho);
  
  G4cout << __METHOD_NAME__ << "Beam properties:"<<G4endl;
  G4cout << __METHOD_NAME__ << "Particle : " 
	 << globals->GetParticleDefinition()->GetParticleName()<<G4endl;
  G4cout << __METHOD_NAME__ << "Mass : " 
	 << globals->GetParticleDefinition()->GetPDGMass()/CLHEP::GeV<< " GeV"<<G4endl;
  G4cout << __METHOD_NAME__ << "Charge : " << charge << " e" << G4endl;
  G4cout << __METHOD_NAME__ << "Total Energy : "
	 << globals->BeamTotalEnergy()/CLHEP::GeV<<" GeV"<<G4endl;
  G4cout << __METHOD_NAME__ << "Kinetic Energy : "
	 << globals->BeamKineticEnergy()/CLHEP::GeV<<" GeV"<<G4endl;
  G4cout << __METHOD_NAME__ << "Momentum : "
	 << globals->BeamMomentum()/CLHEP::GeV<<" GeV"<<G4endl;
  G4cout << __METHOD_NAME__ << "Rigidity (Brho) : "
	 << brho/(CLHEP::tesla*CLHEP::m) << " T*m"<<G4endl;
}

void BDSModularPhysicsList::Cherenkov()
{
  if (!physicsActivated["cherenkov"] && !physicsActivated["cerenkov"])
    {
      constructors.push_back(new BDSPhysicsCherenkov(BDSGlobalConstants::Instance()->MaximumPhotonsPerStep(),
						     BDSGlobalConstants::Instance()->MaximumBetaChangePerStep()));
      physicsActivated["cherenkov"] = true;
      physicsActivated["cerenkov"]  = true;
      if (!physicsActivated["em"])
	{Em();} // requires em physics to work (found empirically)
    }
}

void BDSModularPhysicsList::Em()
{
  ConstructAllLeptons();
  if (!physicsActivated["em"])
    {
      constructors.push_back(new G4EmStandardPhysics());
      physicsActivated["em"] = true;
    }
}

void BDSModularPhysicsList::EmExtra()
{
  ConstructAllLeptons();

  // These are required by GammaNuclear and MuonNuclear which
  // are activated by default in G4EmExtraPhysics.
  ConstructAllShortLived();
  ConstructAllBaryons();
  ConstructAllIons();
  ConstructAllMesons();

  if (!physicsActivated["em_extra"])
    {
      auto constructor = new G4EmExtraPhysics();
#if G4VERSION_NUMBER > 1012
      constructor->Synch(true); // introduced geant version 10.1
#endif
      constructors.push_back(constructor);
      physicsActivated["em_extra"] = true;
    }
}
							  
void BDSModularPhysicsList::EmLow()
{
  ConstructAllLeptons();
  if (!physicsActivated["em_low"])
    {
      constructors.push_back(new G4EmPenelopePhysics());
      physicsActivated["em_low"] = true;
    }
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
							  
void BDSModularPhysicsList::SynchRad()
{
  ConstructAllLeptons();
  if(!physicsActivated["synchrad"])
    {
      constructors.push_back(new BDSPhysicsSynchRad());
      physicsActivated["synchrad"] = true;
    }
}							  
							  
void BDSModularPhysicsList::Muon()
{
  if(!physicsActivated["muon"])
    {
      constructors.push_back(new BDSPhysicsMuon(emWillBeUsed));
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

void BDSModularPhysicsList::SpinDecay()
{
#if G4VERSION_NUMBER > 1020
  if(!physicsActivated["spindecay"])
    {// this will replace regular decay for various processes
      constructors.push_back(new G4SpinDecayPhysics());
      physicsActivated["spindecay"] = true;
    }
#else
  G4cout << G4endl << "Warning: \"spindecay\" physics is only availabe for Geant4.10.2 upwards" << G4endl;
  G4cout << "Using regular decay physics instead" << G4endl;
  Decay();
#endif
}

void BDSModularPhysicsList::CutsAndLimits()
{
  if(!physicsActivated["cutsandlimits"])
    {
      constructors.push_back(new BDSPhysicsCutsAndLimits());
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
  HadronicElastic(); // has to be here to prevent G4 segfault
  if(!physicsActivated["ftfp_bert"])
    {
      constructors.push_back(new G4HadronPhysicsFTFP_BERT());
      physicsActivated["ftfp_bert"] = true;
    }
}

void BDSModularPhysicsList::FTFPBERTHP()
{
  ConstructAllLeptons();
  HadronicElastic(); // has to be here to prevent G4 segfault
  if(!physicsActivated["ftfp_bert_hp"])
    {
      constructors.push_back(new G4HadronPhysicsFTFP_BERT_HP());
      physicsActivated["ftfp_bert_hp"] = true;
    }
}

void BDSModularPhysicsList::LaserWire()
{
  if(!physicsActivated["lw"])
    {
      constructors.push_back(new BDSPhysicsLaserWire());
      physicsActivated["lw"] = true;
    }
}

void BDSModularPhysicsList::BuildAndAttachBiasWrapper(const GMAD::FastList<GMAD::PhysicsBiasing>& biases)
{
  // particles we know we can bias
  std::map<G4String, G4bool> particlesToBias =
    {
      {"e-"     , false},
      {"e+"     , false},
      {"gamma"  , false},
      {"proton" , false},
      {"mu-"    , false},
      {"mu+"    , false},
      {"pi-"    , false},
      {"pi+"    , false}
    };

  // iterate through bias structures and turn on biasing for that particle if it's in the
  // map of acceptable particle definitions.
  for (auto b : biases)
    {
      G4String name = G4String(b.particle);
      if (particlesToBias.find(name) != particlesToBias.end())
	{particlesToBias[name] = true;}
      else
	{
	  G4cerr << __METHOD_NAME__ << "Not possible to bias \"" << name << "\"" << G4endl;
	  exit(1);
	}
    }

  // check whether we need to construct or attach biasing at all
  typedef std::pair<const G4String, G4bool> mapvalue;
  G4bool anyBiases = std::any_of(particlesToBias.begin(),
				 particlesToBias.end(),
				 [](mapvalue i){return i.second;});

  if (!anyBiases)
    {return;}
  else
    {// there are biases
      G4GenericBiasingPhysics* physBias = new G4GenericBiasingPhysics();
      for (auto part : particlesToBias)
	{
	  if (part.second)
	    {
	      G4cout << __METHOD_NAME__ << "wrapping \"" << part.first << "\" for biasing" << G4endl;
	      physBias->Bias(part.first);
	    }
	}
      RegisterPhysics(physBias);
    }
}
