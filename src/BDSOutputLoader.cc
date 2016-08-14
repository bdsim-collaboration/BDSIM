#include "BDSDebug.hh"
#include "BDSOutputLoader.hh"

#include "parser/options.h"

#include "globals.hh" // geant4 types / globals

#include "TFile.h"
#include "TList.h"
#include "TTree.h"


BDSOutputLoader::BDSOutputLoader(G4String filePath):
  validFilePath(false),
  rootEventFile(false)
{
  // open file
  file = new TFile(filePath.c_str(), "READ");
  
  // check it's a valid file
  validFilePath = file->IsZombie();
  if (!validFilePath)
    {G4cout << __METHOD_NAME__ << "No such file \"" << filePath << "\"" << G4endl;}
  else
    {// check it's a rootevent file
      rootEventFile = file->GetListOfKeys()->Contains("Event");
      if (rootEventFile)
	{G4cout << __METHOD_NAME__ << "Not a BDSIM rootevent output format ROOT file" << G4endl;}
    }
}

BDSOutputLoader::~BDSOutputLoader()
{
  delete file; // closes if open
}

GMAD::Options BDSOutputLoader::LoadOptions()
{
  // always change back to this file - assuming other root files could be open
  file->cd();

  GMAD::Options localOptions;
  TTree* optionsTree = (TTree*)file->Get("Options");
  optionsTree->SetBranchAddress("Options.", &localOptions);
  optionsTree->GetEntry(0);

  // Here, we can safely let the TTree* go out of scope
  return localOptions;
}
