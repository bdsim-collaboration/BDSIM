#include "BDSDebug.hh"
#include "BDSOutputLoader.hh"
#include "BDSOutputROOTEventInfo.hh"
#include "BDSOutputROOTEventOptions.hh"

#include "parser/options.h"
#include "parser/optionsBase.h"

#include "globals.hh" // geant4 types / globals

#include "TFile.h"
#include "TList.h"
#include "TTree.h"


BDSOutputLoader::BDSOutputLoader(G4String filePath):
  badFilePath(true),
  rootEventFile(false),
  localOptions(nullptr),
  localEventInfo(nullptr),
  optionsTree(nullptr),
  eventTree(nullptr)
{
  // open file - READ mode to prevent accidental corruption by adding new things
  file = new TFile(filePath.c_str(), "READ");
  
  // check it's a valid file
  badFilePath = file->IsZombie();
  if (badFilePath)
    {
      G4cout << __METHOD_NAME__ << "No such file \"" << filePath << "\"" << G4endl;
      exit(1);
    }
  else
    {// check it's a rootevent file
      rootEventFile = file->GetListOfKeys()->Contains("Event");
      if (!rootEventFile)
	{
	  G4cout << __METHOD_NAME__ << "Not a BDSIM rootevent output format ROOT file" << G4endl;
	  exit(1);
	}
    }

  // set up local structure copies.
  optionsTree = (TTree*)file->Get("Options");
  // Note we don't check on optionsTree pointer as we assume it's valid given
  // we've checked this is a rootevent file.
  localOptions = new BDSOutputROOTEventOptions();
  optionsTree->SetBranchAddress("Options.", &localOptions); 
  
  eventTree = (TTree*)file->Get("Event");
  localEventInfo = new BDSOutputROOTEventInfo();
  eventTree->SetBranchAddress("Info.", &localEventInfo);
}

BDSOutputLoader::~BDSOutputLoader()
{
  delete file; // closes if open
  delete localOptions;
  delete localEventInfo;
}

GMAD::OptionsBase BDSOutputLoader::OptionsBaseClass()
{
  // always change back to this file - assuming other root files could be open
  file->cd();
  optionsTree->GetEntry(0);
  return *(static_cast<GMAD::OptionsBase*>(localOptions));
}

GMAD::Options BDSOutputLoader::Options()
{
  return GMAD::Options(OptionsBaseClass());
}

G4String BDSOutputLoader::SeedState(G4int eventNumber)
{
  // always change back to this file - assuming other root files could be open
  file->cd();
  
  eventTree->GetEntry((int)eventNumber);
  
  return G4String(localEventInfo->seedStateAtStart);
}
