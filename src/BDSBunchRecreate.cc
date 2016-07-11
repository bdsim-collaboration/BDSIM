#include "BDSBunchRecreate.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSOutputROOTEventInfo.hh"
#include "BDSOutputROOTEventSampler.hh"
#include "BDSRandom.hh"

#include "analysis/Event.hh"
#include "parser/options.h"

#include "globals.hh"  // geant4 types / globals
#include "G4ParticleTable.hh"

#include "TChain.h"

#include <string>
#include <vector>

BDSBunchRecreate::BDSBunchRecreate():
  filename(""),
  eventOffset(0),
  currentEventNumber(0),
  entries(0),
  strongRecreation(true),
  chain(nullptr),
  event(nullptr)
{;}

BDSBunchRecreate::~BDSBunchRecreate()
{
  delete chain;
  delete event;
}

void BDSBunchRecreate::SetOptions(const GMAD::Options& options)
{
  filename           = options.distribFile;
  eventOffset        = options.eventOffset;
  currentEventNumber = eventOffset;
  strongRecreation   = options.recreateSeedState;

  LoadFile();
}

void BDSBunchRecreate::LoadFile()
{
  TChain* chain = new TChain("Event");
  
  G4int nFilesAdded = chain->Add(filename.c_str());
  if (nFilesAdded == 0)
    {
      G4cout << __METHOD_NAME__ << "Input ROOT file not found \"" << filename << "\"" << G4endl;
      exit(1);
    }

  entries = chain->GetEntries();
  
  event = new Event();
  // prepare vector of samplers to load - empty vector means none.
  std::vector<std::string> names;
  event->SetBranchAddress(chain, names);

  // make access efficient by avoiding copying all sampler data each time
  chain->SetBranchStatus("*", 0);       // turn off all branches
  chain->SetBranchStatus("Primary", 1); // turn on primary branch
  chain->SetBranchStatus("Info",    1); // turn on info branch
  
  // Here we set the particle name from the PDG ID of first useful event in the file.
  // As the bunch is constructed before BDSModularPhysics list this is ok. BDSModularPhysicsList
  // access global constants for the particle name and constructs the particle definition used
  // for every event.
  chain->GetEntry(currentEventNumber); // load first useful event
  G4int particlePDGID = event->GetPrimaries()->partID[0]; // get PDG ID of first entry
  G4String particleName = G4ParticleTable::GetParticleTable()->FindParticle(particlePDGID)->GetParticleName();
  BDSGlobalConstants::Instance()->SetParticleName(particleName);
}

void BDSBunchRecreate::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
				       G4double& xp, G4double& yp, G4double& zp,
				       G4double& t , G4double&  E, G4double& weight)
{
  if (currentEventNumber > entries)
    {
      G4cout << __METHOD_NAME__ << "reached end of primary entries - repeating" << G4endl;
      currentEventNumber = 0;
    }
  
  // load the entry
  chain->GetEntry(currentEventNumber);

  // load the seed state
  if (strongRecreation)
    {BDSRandom::SetSeedState(event->info->seedStateAtStart);}

  // set the variables - the type is templated so the auto helps here
  const auto& p = *(event->GetPrimaries());
  // primaries is an instance of BDSOutputROOTEventSampler with only one entry
  // all members are vectors - take just first entry - ie index 0

  // Note, there's no need to use the curvilinear transform as that would've already
  // been used when the coordinates were generated and before they were recorded, so
  // we simply load the basic global coordinates directly.
  x0 = p.x[0];
  y0 = p.y[0];
  z0 = p.z;
  xp = p.xp[0];
  yp = p.yp[0];
  zp = p.zp[0];
  t  = p.t[0];
  weight = p.weight[0];
  E      = p.energy[0]; // total energy in both cases
  
  // increment
  currentEventNumber++;
}
