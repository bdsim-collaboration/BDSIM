#ifndef BDSOUTPUTLOADER_H
#define BDSOUTPUTLOADER_H

#include "parser/options.h"
#include "parser/optionsBase.h"

#include "globals.hh" // geant4 types / globals

class BDSOutputROOTEventInfo;
class BDSOutputROOTEventOptions;
class TFile;
class TTree;

/**
 * @brief Loader of ROOT Event output for receating events.
 *
 * @author Laurie Nevay
 */

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

  BDSOutputROOTEventOptions* localOptions;
  BDSOutputROOTEventInfo*    localEventInfo;

  TTree* optionsTree;
  TTree* eventTree;
};

#endif
