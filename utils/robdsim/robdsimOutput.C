#include "robdsimOutput.h"

#include <iostream>
#include <fstream>

#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "TList.h"
#include "TChain.h"
#include "TFile.h"

robdsimOutput::robdsimOutput() {
}

robdsimOutput::robdsimOutput(char *path, bool debugIn) {
  debug = debugIn;
  this->MakeListOfRootFiles(path);
  this->MakeListOfSamplers();
  this->CommonCtor();
  this->Chain();
}


robdsimOutput::~robdsimOutput() {
  delete primaryChain;
  delete elossChain;
  delete plossChain;
  delete pelossChain;
}

void robdsimOutput::CommonCtor() {
  primaryChain = new TChain("primaries");
  elossChain   = new TChain("ElossTree");
  plossChain   = new TChain("PlossTree");
  pelossChain  = new TChain("PrecisionRegionElossTree");  
}

void robdsimOutput::MakeListOfRootFiles(char *path) {
  TSystemFile sf = TSystemFile(path,path);
  std::vector<std::string> fileNames;  
  /**************************************************
   Complete directory load 
   *************************************************/
  if(sf.IsDirectory()) {
    TSystemDirectory dir(path,path);
    TList *fnl = dir.GetListOfFiles();
    TSystemFile *file;
    TIter next(fnl);
    while ((file = (TSystemFile*)next())) {
      TString fname = file->GetName(); 
      if (!file->IsDirectory() && fname.EndsWith(".root")) {
	fileNames.push_back((std::string)fname.Data());
      }
    }
  }
  /**************************************************
   File load  
   *************************************************/
  else {
    std::ifstream ifs(path);
    std::string l;
    while(ifs.good()) {
      l.clear();
      std::getline(ifs, l);
      if(l[0] != '#') {       
	fileNames.push_back(l);
      }
    }
    ifs.close();
  }
  rootFiles = fileNames;
}

void robdsimOutput::MakeListOfSamplers() { 
  if(debug) {
    std::cout << "robdsimOutput::MakeListOfSamplers> Template "<< rootFiles[0].data() << std::endl;
  }


  TFile *f = new TFile(rootFiles[0].data());
  
  delete f;
}

void robdsimOutput::Chain() {
  // Loop over files 
  for(std::vector<std::string>::iterator f = rootFiles.begin(); f != rootFiles.end(); ++f) {
    
    // Chain trees 
    if(debug) 
      std::cout << "robdsimOutput::Chain> " << (*f).data() << std::endl;
    primaryChain->Add((*f).data()); 
    elossChain->Add((*f).data()); 
    plossChain->Add((*f).data()); 
    pelossChain->Add((*f).data());

    // Loop over samplers and chain 
    
    // Add histograms 
    
  }
  
}
  
void robdsimOutput::ElossLoop() {
  for(int i=0;i<elossChain->GetEntries();i++) {
    elossChain->GetEntry(i);
  }
}
 
void robdsimOutput::PlossLoop() {
}

void robdsimOutput::PrecisionElossLoop() {
}

void robdsimOutput::SamplerLoop() {
}
