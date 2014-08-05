#ifndef SamplerManager_h
#define SamplerManager_h


#include <vector> 

#include "TString.h"
#include "TFile.h"
#include "TTree.h"

class SamplerManager {
 public : 
  SamplerManager(TString fileName);
  ~SamplerManager();

  void Init();
  void PrintTrees(); 
  
 private:
  TString fileName;
  TFile  *file;  
  std::vector<TString> treeNames; 
  std::vector<TTree*>  trees;
};


#endif
