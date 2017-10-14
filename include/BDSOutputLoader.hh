#ifndef BDSOUTPUTLOADER_H
#define BDSOUTPUTLOADER_H

#include "globals.hh" // geant4 types / globals

namespace GMAD {
  class Beam;
  class BeamBase;
  class Options;
  class OptionsBase;
}

class BDSOutputROOTEventBeam;
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
  explicit BDSOutputLoader(G4String filePath);
  ~BDSOutputLoader();

  GMAD::OptionsBase OptionsBaseClass();
  GMAD::Options     Options();

  GMAD::BeamBase BeamBaseClass();
  GMAD::Beam     Beam();

  G4String SeedState(G4int eventNumber = 0);
  
private:
  BDSOutputLoader() = delete;
  BDSOutputLoader(const BDSOutputLoader&) = delete;
  BDSOutputLoader& operator=(const BDSOutputLoader&) = delete;

  TFile* file;

  G4bool badFilePath;
  G4bool rootEventFile;

  BDSOutputROOTEventBeam*    localBeam;
  BDSOutputROOTEventOptions* localOptions;
  BDSOutputROOTEventInfo*    localEventInfo;

  TTree* beamTree;
  TTree* optionsTree;
  TTree* eventTree;
};

#endif
