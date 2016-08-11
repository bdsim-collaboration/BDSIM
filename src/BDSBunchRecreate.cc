#include "BDSBunchRecreate.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSOutputROOTEventInfo.hh"
#include "BDSOutputROOTEventSampler.hh"
#include "BDSRandom.hh"

#include "parser/options.h"

#include "globals.hh"  // geant4 types / globals
#include "G4ParticleTable.hh"

#include "TFile.h"
#include "TTree.h"

#include <string>
#include <vector>

BDSBunchRecreate::BDSBunchRecreate():
  eventOffset(0),
  currentEventNumber(0),
  entries(0),
  strongRecreation(true),
  file(nullptr),
  eventTree(nullptr),
  primary(nullptr),
  info(nullptr),
  primaryLocal(nullptr),
  infoLocal(nullptr)
{;}

BDSBunchRecreate::~BDSBunchRecreate()
{
  if (file)
    {file->Close();}
}

void BDSBunchRecreate::SetOptions(const GMAD::Options& options)
{
  eventOffset        = options.eventOffset;
  currentEventNumber = eventOffset;
  strongRecreation   = options.recreateSeedState;

  LoadFile(options.distribFile);
}

void BDSBunchRecreate::LoadFile(G4String filename)
{
  file = new TFile(filename.c_str(), "READ");
  if (file->IsZombie())
    {
      G4cout << __METHOD_NAME__ << "Input ROOT file not found \"" << filename << "\"" << G4endl;
      exit(1);
    }

  eventTree = (TTree*)file->Get("Event");
  eventTree->GetEntry(0);
  entries = (G4int) eventTree->GetEntries();

  // prepare vector of samplers to load - empty vector means none.
  std::vector<std::string> names;

#ifndef __ROOTDOUBLE__
  primaryLocal = new BDSOutputROOTEventSampler<float>();
#else
  primaryLocal = new BDSOutputROOTEventSampler<double>();
#endif
  primaryLocal->Flush();
  //infoLocal = new BDSOutputROOTEventInfo();
  //info      = eventTree->GetBranch("Info.");
  //info->SetAddress(infoLocal);
  //primary = eventTree->GetBranch("Primary.");
  //primary->SetAddress(primaryLocal);

  eventTree->SetBranchAddress("Primary.",&primaryLocal);
  
  // Here we set the particle name from the PDG ID of first useful event in the file.
  // As the bunch is constructed before BDSModularPhysics list this is ok. BDSModularPhysicsList
  // access global constants for the particle name and constructs the particle definition used
  // for every event.
  //G4cout << "N Entries " << primary->GetEntries() << G4endl;
  eventTree->GetEntry(2); // load first useful event
  G4int particlePDGID = primaryLocal->partID[0]; // get PDG ID of first entry
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

  // load the seed state
  if (strongRecreation)
    {
      gDirectory->pwd();
      file->cd();
      gDirectory->pwd();
      info->GetEntry(0);
      BDSRandom::SetSeedState(infoLocal->seedStateAtStart);
    }

  // load the entry
  primary->GetEntry(currentEventNumber);
  // Primary is an instance of BDSOutputROOTEventSampler with only one entry
  // all members are vectors - take just first entry - ie index 0
  // Note, there's no need to use the curvilinear transform as that would've already
  // been used when the coordinates were generated and before they were recorded, so
  // we simply load the basic global coordinates directly.
  x0 = primaryLocal->x[0];
  y0 = primaryLocal->y[0];
  z0 = primaryLocal->z;
  xp = primaryLocal->xp[0];
  yp = primaryLocal->yp[0];
  zp = primaryLocal->zp[0];
  t  = primaryLocal->t[0];
  weight = primaryLocal->weight[0];
  E      = primaryLocal->energy[0]; // total energy in both cases
  
  // increment
  currentEventNumber++;
}
