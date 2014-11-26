#ifndef __RoBdsimOutput_h

#include <vector>

#include "TChain.h"
#include "TTree.h"
#include "TFile.h"

#include "Sampler.h"
#include "Eloss.h"
#include "PrecisionEloss.h"

class robdsimOutput {

 public : 
  robdsimOutput();
  robdsimOutput(char *path, bool debug);   
  ~robdsimOutput(); 

  void CommonCtor(); 
  void MakeListOfRootFiles(char *path); 
  std::vector<std::string> GetListOfRootFiles() { return rootFiles; }
  void MakeListOfSamplers();
  std::vector<std::string> GetListOfSamplers() { return samplerNames;}
  void Chain();
  
  void ElossLoop(); 
  void PlossLoop();
  void PrecisionElossLoop();
  void SamplerLoop(); 

 private: 
  bool   debug;
  char   *path;
  std::vector<std::string> rootFiles; 
  std::vector<std::string> samplerNames;

  TChain *primaryChain; 
  TChain *elossChain;
  TChain *plossChain;
  TChain *pelossChain;
  std::vector<TChain*> samplerChains;

  Sampler        primary; 
  Sampler        sampler; 
  Eloss          eloss;
  Eloss          ploss;
  PrecisionEloss peloss;

};

#endif
