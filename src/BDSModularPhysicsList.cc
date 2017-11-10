#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIonDefinition.hh"
#include "BDSModularPhysicsList.hh"
#include "BDSParticleDefinition.hh"
#include "BDSPhysicalConstants.hh"
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
#include "G4EmLivermorePhysics.hh"
#include "G4EmLivermorePolarizedPhysics.hh"
#include "G4EmLowEPPhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4EmStandardPhysics.hh"
#if G4VERSION_NUMBER > 1019
#include "G4EmStandardPhysicsGS.hh"
#endif
#include "G4EmStandardPhysicsSS.hh"
#include "G4EmStandardPhysicsWVI.hh"
#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronPhysicsFTFP_BERT.hh"
#include "G4HadronPhysicsFTFP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BIC.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4IonBinaryCascadePhysics.hh"
#include "G4IonINCLXXPhysics.hh"
#include "G4IonPhysics.hh"
#if G4VERSION_NUMBER > 1022
#include "G4IonPhysicsPHP.hh"
#endif
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

#include <iomanip>
#include <iterator>
#include <limits>
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
  physicsConstructors.insert(std::make_pair("em_low",           &BDSModularPhysicsList::EmPenelope));  // alias
  physicsConstructors.insert(std::make_pair("em_penelope",      &BDSModularPhysicsList::EmPenelope));
  physicsConstructors.insert(std::make_pair("em_livermore",     &BDSModularPhysicsList::EmLivermore));
  physicsConstructors.insert(std::make_pair("em_livermore_polarised", &BDSModularPhysicsList::EmLivermorePolarised));
  physicsConstructors.insert(std::make_pair("em_low_ep",        &BDSModularPhysicsList::EmLowEP));
#if G4VERSION_NUMBER > 1019
  physicsConstructors.insert(std::make_pair("em_gs",            &BDSModularPhysicsList::EmGS));
#endif
  physicsConstructors.insert(std::make_pair("em_ss",            &BDSModularPhysicsList::EmSS));
  physicsConstructors.insert(std::make_pair("em_wvi",           &BDSModularPhysicsList::EmWVI));
  physicsConstructors.insert(std::make_pair("em_1",             &BDSModularPhysicsList::Em1));
  physicsConstructors.insert(std::make_pair("em_2",             &BDSModularPhysicsList::Em2));
  physicsConstructors.insert(std::make_pair("em_3",             &BDSModularPhysicsList::Em3));
  physicsConstructors.insert(std::make_pair("em_4",             &BDSModularPhysicsList::Em4));
  physicsConstructors.insert(std::make_pair("hadronic_elastic", &BDSModularPhysicsList::HadronicElastic));
  physicsConstructors.insert(std::make_pair("hadronic",         &BDSModularPhysicsList::QGSPBERT));
  physicsConstructors.insert(std::make_pair("hadronic_hp",      &BDSModularPhysicsList::QGSPBERTHP));
  physicsConstructors.insert(std::make_pair("ion",              &BDSModularPhysicsList::Ion));
#if G4VERSION_NUMBER > 1022
  physicsConstructors.insert(std::make_pair("ionphp",           &BDSModularPhysicsList::IonPHP));
#endif
  physicsConstructors.insert(std::make_pair("ioninclxx",        &BDSModularPhysicsList::IonINCLXX));
  physicsConstructors.insert(std::make_pair("ionbinary",        &BDSModularPhysicsList::IonBinary));
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
}

void BDSModularPhysicsList::ConstructProcess()
{
  G4VModularPhysicsList::ConstructProcess();
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

  auto particle = G4ParticleTable::GetParticleTable()->FindParticle(particleName);
  if (!particle)
    {// could be ion that isn't defined
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "primary particle not defined yet - could be ion" << G4endl;
#endif
      return;
    } 
  else
    {
      auto pl = particle->GetProcessManager()->GetProcessList();
      for (G4int i = 0; i < pl->length(); i++)
	{ G4cout << "\"" << (*pl)[i]->GetProcessName() << "\"" << G4endl; }
    }
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
  G4GenericIon::GenericIonDefinition();
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

BDSParticleDefinition* BDSModularPhysicsList::ConstructBeamParticle(G4String particleName,
								    G4double totalEnergy,
								    G4double ffact) const
{
  particleName.toLower();

  BDSParticleDefinition* particleDefB = nullptr; // result can be constructed in two ways
  
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  
  if (particleName.contains("ion"))
    {
      G4GenericIon::GenericIonDefinition(); // construct general ion particle
      auto ionDef = new BDSIonDefinition(particleName); // parse the ion definition

      G4IonTable* ionTable = particleTable->GetIonTable();
      G4double mass   = ionTable->GetIonMass(ionDef->Z(), ionDef->A());
      G4double charge = ionDef->Charge(); // correct even if overridden
      particleDefB = new BDSParticleDefinition(particleName, mass, charge,
					       totalEnergy, ffact, ionDef);
      // this takes ownership of the ion definition
    }
  else
    {
      ConstructBeamParticleG4(particleName);
      auto particleDef = particleTable->FindParticle(particleName);
      if (!particleDef)
	{
	  G4cerr << "Particle \"" << particleName << "\" not found: quitting!" << G4endl;
	  exit(1);
	}
      particleDefB = new BDSParticleDefinition(particleDef, totalEnergy, ffact);
    }

  globals->SetBeamParticleDefinition(particleDefB); // export for bunch distribution
  return particleDefB;
}

void BDSModularPhysicsList::ConstructBeamParticleG4(G4String name) const
{
  if (name == "proton")
    {G4Proton::ProtonDefinition();}
  else if (name == "antiproton")
    {G4AntiProton::AntiProtonDefinition();}
  else if (name == "e-")
    {G4Electron::ElectronDefinition();}
  else if (name == "e+")
    {G4Positron::PositronDefinition();}
  else if (name == "pi-")
    {G4PionMinus::PionMinusDefinition();}
  else if (name == "pi+")
    {G4PionPlus::PionPlusDefinition();}
  else if (name == "neutron")
    {G4Neutron::NeutronDefinition();}
  else if (name == "photon" || name == "gamma")
    {G4Gamma::Gamma();}
  else if (name == "mu-")
    {G4MuonMinus::MuonMinusDefinition();}
  else if (name == "mu+")
    {G4MuonPlus::MuonPlusDefinition();}
  else
    {G4cerr << "Unknown particle type \"" << name << "\"" << G4endl;}
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
							  
void BDSModularPhysicsList::EmPenelope()
{
  ConstructAllLeptons();
  if (!physicsActivated["em_penelope"])
    {
      constructors.push_back(new G4EmPenelopePhysics());
      physicsActivated["em_penelope"] = true;
    }
}

void BDSModularPhysicsList::EmLivermore()
{
  ConstructAllLeptons();
  if (!physicsActivated["em_livermore"] || !physicsActivated["em_low"])
    {
      constructors.push_back(new G4EmLivermorePhysics());
      physicsActivated["em_livermore"] = true;
      physicsActivated["em_low"]       = true;
    }
}

void BDSModularPhysicsList::EmLivermorePolarised()
{
  ConstructAllLeptons();
  if (!physicsActivated["em_livermore_polarised"])
    {
      constructors.push_back(new G4EmLivermorePolarizedPhysics());
      physicsActivated["em_low"] = true;
    }
}

void BDSModularPhysicsList::EmLowEP()
{
  ConstructAllLeptons();
  if (!physicsActivated["em_low_ep"])
    {
      constructors.push_back(new G4EmLowEPPhysics());
      physicsActivated["em_low_ep"] = true;
    }
}

#if G4VERSION_NUMBER > 1019
void BDSModularPhysicsList::EmGS()
{
  ConstructAllLeptons();
  if (!physicsActivated["em_gs"])
    {
      constructors.push_back(new G4EmStandardPhysicsGS());
      physicsActivated["em_gs"] = true;
    }
}
#endif

void BDSModularPhysicsList::EmSS()
{
  ConstructAllLeptons();
  if (!physicsActivated["em_ss"])
    {
      constructors.push_back(new G4EmStandardPhysicsSS());
      physicsActivated["em_ss"] = true;
    }
}

void BDSModularPhysicsList::EmWVI()
{
  ConstructAllLeptons();
  if (!physicsActivated["em_wvi"])
    {
      constructors.push_back(new G4EmStandardPhysicsWVI());
      physicsActivated["em_wvi"] = true;
    }
}

void BDSModularPhysicsList::Em1()
{
  ConstructAllLeptons();
  if (!physicsActivated["em_1"])
    {
      constructors.push_back(new G4EmStandardPhysics_option1());
      physicsActivated["em_1"] = true;
    }
}

void BDSModularPhysicsList::Em2()
{
  ConstructAllLeptons();
  if (!physicsActivated["em_2"])
    {
      constructors.push_back(new G4EmStandardPhysics_option2());
      physicsActivated["em_2"] = true;
    }
}

void BDSModularPhysicsList::Em3()
{
  ConstructAllLeptons();
  if (!physicsActivated["em_3"])
    {
      constructors.push_back(new G4EmStandardPhysics_option3());
      physicsActivated["em_3"] = true;
    }
}

void BDSModularPhysicsList::Em4()
{
  ConstructAllLeptons();
  if (!physicsActivated["em_4"])
    {
      constructors.push_back(new G4EmStandardPhysics_option4());
      physicsActivated["em_4"] = true;
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

void BDSModularPhysicsList::Ion()
{
  ConstructAllBaryons();
  ConstructAllIons();
  ConstructAllLeptons();
  ConstructAllMesons();
  ConstructAllShortLived();

  if (!physicsActivated["ion"])
    {
      constructors.push_back(new G4IonPhysics());
      physicsActivated["ion"] = true;
    }
}

#if G4VERSION_NUMBER > 1022
void BDSModularPhysicsList::IonPHP()
{
  BDS::CheckLowEnergyDataExists("ionphp");
  ConstructAllBaryons();
  ConstructAllIons();
  ConstructAllLeptons();
  ConstructAllMesons();
  ConstructAllShortLived();
  
  if (!physicsActivated["ionphp"])
    {
      constructors.push_back(new G4IonPhysicsPHP());
      physicsActivated["ionphp"] = true;
    }
}
#endif

void BDSModularPhysicsList::IonINCLXX()
{
  BDS::CheckLowEnergyDataExists("ioninclxx");
  ConstructAllBaryons();
  ConstructAllIons();
  ConstructAllLeptons();
  ConstructAllMesons();
  ConstructAllShortLived();
  
  if (!physicsActivated["ioninclxx"])
    {
      constructors.push_back(new G4IonINCLXXPhysics());
      physicsActivated["ioninclxx"] = true;
    }
}

void BDSModularPhysicsList::IonBinary()
{
  BDS::CheckLowEnergyDataExists("ionbinary");
  ConstructAllBaryons();
  ConstructAllIons();
  ConstructAllLeptons();
  ConstructAllMesons();
  ConstructAllShortLived();
  
  if (!physicsActivated["ionbinary"])
    {
      constructors.push_back(new G4IonBinaryCascadePhysics());
      physicsActivated["ionbinary"] = true;
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
