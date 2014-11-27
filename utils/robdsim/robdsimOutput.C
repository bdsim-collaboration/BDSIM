#include "robdsimOutput.h"

#include <iostream>
#include <fstream>

#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "TList.h"
#include "TChain.h"
#include "TFile.h"
#include "TObject.h"


/***********************************************************************************************************/
robdsimOutput::robdsimOutput() {
}

/***********************************************************************************************************/
robdsimOutput::robdsimOutput(char *path, bool debugIn) {
  debug = debugIn;
  this->MakeListOfRootFiles(path);
  this->MakeListOfSamplers();
  this->CommonCtor();
  this->Chain();
  this->AssignStructures(); // all but samplers
}

/***********************************************************************************************************/
robdsimOutput::~robdsimOutput() {
  delete primaryChain;
  delete elossChain;
  delete plossChain;
  delete pelossChain;

  // loop over sampler chains and delete
  for(std::vector<TChain*>::iterator c = samplerChains.begin(); c != samplerChains.end(); c++) {
    delete *c;
  }
}
/***********************************************************************************************************/
void robdsimOutput::CommonCtor() {

  // primary 
  primaryChain = new TChain("primaries");
  // energy loss 
  elossChain   = new TChain("ElossTree");
  plossChain   = new TChain("PlossTree");
  pelossChain  = new TChain("PrecisionRegionElossTree");  
  // loop over sampler names and create chains 
  for(std::vector<std::string>::iterator i = samplerNames.begin(); i != samplerNames.end(); ++i) { 
    samplerChains.push_back(new TChain((*i).data()));
  }
}

/***********************************************************************************************************/
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
   Single root file load  
   *************************************************/
  else if(TString(path).EndsWith(".root")) {
    fileNames.push_back(path);
  }
  /**************************************************
   File listing file load  
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

/***********************************************************************************************************/
void robdsimOutput::MakeListOfSamplers() { 
  if(debug) {
    std::cout << "robdsimOutput::MakeListOfSamplers> Template "<< rootFiles[0].data() << std::endl;
  }


  TFile *f  = new TFile(rootFiles[0].data());
  TList *fk = f->GetListOfKeys();
  
  for(int i = 0; i<fk->GetEntries(); i++) {
    TObject  *ko = fk->At(i);             // key object
    TObject  *fo = f->Get(ko->GetName()); // file object 
    if(std::string(fo->ClassName()) == "TTree" && std::string(ko->GetName()).find("Sampler") != std::string::npos ) {
      samplerNames.push_back(std::string(ko->GetName()));      
    }
  }
  
  delete f;
}

void robdsimOutput::MakeHistograms() { 
  return;
}


/***********************************************************************************************************/
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
    for(std::vector<TChain*>::iterator c = samplerChains.begin(); c != samplerChains.end(); c++) {
      (*c)->Add((*f).data());
    }

    // Add histograms 
    
  }  
}
  
/***********************************************************************************************************/
void robdsimOutput::AssignStructures() {
  primary = new Sampler(this->primaryChain);
  eloss   = new Eloss(this->elossChain);
  ploss   = new Eloss(this->plossChain); 
  peloss  = new PrecisionEloss(this->pelossChain);
}

/***********************************************************************************************************/
void robdsimOutput::ElossLoop() {
  for(int i=0;i<elossChain->GetEntries();i++) {
    elossChain->GetEntry(i);
  }
}
 
/***********************************************************************************************************/
void robdsimOutput::PlossLoop() {
  for(int i=0;i<plossChain->GetEntries();i++) {
    plossChain->GetEntry(i);
  }
}

/***********************************************************************************************************/
void robdsimOutput::PrecisionElossLoop() {
  for(int i=0;i<pelossChain->GetEntries();i++) {
    pelossChain->GetEntry(i);
  }
}

/***********************************************************************************************************/
void robdsimOutput::SamplerLoop() {
  // loop over samplers 
  for(std::vector<TChain*>::iterator c = samplerChains.begin(); c != samplerChains.end(); c++) {
    Sampler         s = Sampler(*c);    
    SamplerAnalysis a = SamplerAnalysis(&s);  
    
    std::cout << "robdsimOutput::SamplerLoop> " << (*c)->GetName() << " " << (*c)->GetEntries() << std::endl;;
    // loop of sampler entries
    for(int i=0;i<(*c)->GetEntries();i++) {
      (*c)->GetEntry(i);
      a.ProcessEntry();
    }
  }  
}
