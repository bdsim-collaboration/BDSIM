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
  robdsimOutput(char *path);   
  ~robdsimOutput(); 

  void Chain();
  
  void ElossLoop(); 
  void PlossLoop();
  void PrecisionElossLoop();
  void SamplerLoop(); 

 private: 
  char *path; 
  TChain *primaryChain; 
  TChain *elossChain;
  TChain *plossChain;
  TChain *precisionElossChain;
  std::vector<TChain*> samplerChains; 

  Sampler        primary; 
  Sampler        sampler; 
  Eloss          eloss;
  Eloss          ploss;
  PrecisionEloss peloss;

};

#endif
