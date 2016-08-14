#ifndef BDSOUTPUTLOADER_H
#define BDSOUTPUTLOADER_H

#include "parser/options.h"

#include "globals.hh" // geant4 types / globals

class TFile;

class BDSOutputLoader
{
public:
  BDSOutputLoader(G4String filePath);
  ~BDSOutputLoader();

  GMAD::Options LoadOptions();
  
private:
  BDSOutputLoader();

  TFile* file;

  G4bool validFilePath;
  G4bool rootEventFile;
};

#endif
