#ifndef SamplerManager_h
#define SamplerManager_h


#include <vector> 
#include <iostream>

#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TList.h"
#include "TKey.h"
#include "TChain.h"

#include "Sampler.h"

class SamplerManager {
 public : 
  SamplerManager();
  SamplerManager(TString fileName);
  ~SamplerManager();

  void AddFile(TString fileName);
  void Init();
  void PrintTrees(); 
  void PrintFiles();
  void MakeSingleTree(); 

 private:
  TFile  *file;  
  std::vector<TString>  fileNames;
  std::vector<TString>  treeNames; 
  std::vector<TFile*>   files;
  std::vector<TChain*>  chains;
  std::vector<Sampler*> samplers;
};


#endif
