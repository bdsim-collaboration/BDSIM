#ifndef __RoBdsimOutput_h

#include <vector>
#include <map>

#include "TChain.h"
#include "TTree.h"
#include "TFile.h"
#include "TH1D.h"

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
  void MakeHistograms();
  std::vector<std::string> GetListOfHistoNames() { return histoNames;} 
  void Chain();
  std::vector<TChain*> GetListOfSamplerChains() { return samplerChains;}
  
  void ElossLoop(); 
  void PlossLoop();
  void PrecisionElossLoop();
  void SamplerLoop(); 

  void AddPrimaryHisto(TH1D *h) {primaryHistos[h->GetName()] = h;}
  void AddElossHisto(TH1D *h)   {elossHistos[h->GetName()]   = h;}
  void AddPlossHisto(TH1D *h)   {plossHistos[h->GetName()]   = h;}
  void AddPelossHisto(TH1D *h)  {pelossHistos[h->GetName()]  = h;} 
 
  TH1D* GetPrimaryHisto(std::string name) { return primaryHistos[name];}
  TH1D* GetElossHisto(std::string name)   { return elossHistos[name];  }
  TH1D* GetPlossHisto(std::string name)   { return plossHistos[name];  }
  TH1D* GetPelossHisto(std::string name)  { return pelossHistos[name]; }
  TH1D* GetHisto(std::string name)        { return histos[name]; }

 private: 
  // Internal variables 
  bool   debug;
  char   *path;

  // File and samplers
  std::vector<std::string> rootFiles; 
  std::vector<std::string> samplerNames;
  std::vector<std::string> histoNames;
  

  // Chains of files 
  TChain *primaryChain; 
  TChain *elossChain;
  TChain *plossChain;
  TChain *pelossChain;
  std::vector<TChain*> samplerChains;


  // Histograms and outputx
  std::map<std::string,TH1D*>     primaryHistos;
  std::map<std::string,TH1D*>     elossHistos;
  std::map<std::string,TH1D*>     plossHistos; 
  std::map<std::string,TH1D*>     pelossHistos;
  std::map<std::string,TH1D*>     histos;

  // Event data structures 
  Sampler        primary; 
  Sampler        sampler; 
  Eloss          eloss;
  Eloss          ploss;
  PrecisionEloss peloss;

};

#endif
