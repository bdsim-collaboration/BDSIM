#include "SamplerManager.h"

#include <iostream>

SamplerManager::SamplerManager(TString fileNameIn) : fileName(fileNameIn) 
{
  std::cout << fileName << std::endl;
  file = new TFile(fileName);
  Init();
}

SamplerManager::~SamplerManager() {
  delete file;
}

void SamplerManager::Init() {
  // loop over available trees in file
  
}

void SamplerManager::PrintTrees() { 
  
}
