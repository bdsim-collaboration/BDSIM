#include "SamplerManager.h"


SamplerManager::SamplerManager() 
{  
}

SamplerManager::SamplerManager(TString fileName)  
{
  AddFile(fileName);
  Init();
}

SamplerManager::~SamplerManager() {
}

void SamplerManager::AddFile(TString fileName) { 
  TFile *f = new TFile(fileName); 
  files.push_back(f);
  fileNames.push_back(fileName);
}

void SamplerManager::Init() {
  // loop over available trees in file
  TList *listOfKeys = files[0]->GetListOfKeys();
  for(int i=0;i<listOfKeys->GetSize();i++) {
    TKey *key = (TKey*)listOfKeys->At(i);
    const char *className = key->GetClassName();
    const char *objName   = key->GetName(); 
    const char *objTitle  = key->GetTitle(); 
#ifdef DEBUG        
    std::cout << i << " " << className << " \"" << objTitle << "\" " << objName;
#endif 
    // check for tree and sampler output
    if(TString(className) != "TTree" || TString(objTitle) != "Sampler output")  {
#ifdef DEBUG
      std::cout << std::endl;      
#endif 
      continue; 
    }
    else {
#ifdef DEBUG
      std::cout << " ADDING TO MANAGER" << std::endl;
#endif
    }

    // Add tree to maps
    treeNames.push_back(objName);
  }

  // Loop over trees and files to form chains 
  for(std::vector<TString>::iterator tit = treeNames.begin(); tit != treeNames.end(); tit++) {
    TChain *c = new TChain(*tit);    
    for(std::vector<TString>::iterator fit = fileNames.begin(); fit != fileNames.end(); fit++) { 
      c->Add(*fit);
    }
    chains.push_back(c);
  }
}

void SamplerManager::PrintTrees() { 
  for(std::vector<TString>::iterator it = treeNames.begin(); it != treeNames.end(); it++) {
    std::cout << *it << std::endl;
  }
}

void SamplerManager::PrintFiles() { 
  for(std::vector<TString>::iterator it = fileNames.begin(); it != fileNames.end(); it++) {
    std::cout << *it << std::endl;
  }  
}

void SamplerManager::MakeSingleTree() {
  for(std::vector<TChain*>::iterator it = chains.begin(); it != chains.end(); it++) { 
    std::cout << (*it)->GetEntries() << std::endl;
  }

  for(int i=0;i<(int)chains.size();i++) { 
    Sampler *s = new Sampler(chains[i]);
    samplers.push_back(s);
 }
    
  for(int i=0;i<(int)chains[0]->GetEntries();i++) { 
    
  }
}
