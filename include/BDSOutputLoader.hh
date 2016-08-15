#ifndef BDSOUTPUTLOADER_H
#define BDSOUTPUTLOADER_H

#include "parser/options.h"
#include "parser/optionsBase.h"

#include "globals.hh" // geant4 types / globals

#include <sstream>

class BDSOutputROOTEventInfo;
class TFile;
class TTree;

class BDSOutputLoader
{
public:
  BDSOutputLoader(G4String filePath);
  ~BDSOutputLoader();

  GMAD::OptionsBase OptionsBaseClass();
  GMAD::Options     Options();

  G4String SeedState(G4int eventNumber = 0);
  
private:
  BDSOutputLoader();

  TFile* file;

  G4bool badFilePath;
  G4bool rootEventFile;

  GMAD::OptionsBase*      localOptions;
  BDSOutputROOTEventInfo* localEventInfo;

  TTree* optionsTree;
  TTree* eventTree;
};

#endif
