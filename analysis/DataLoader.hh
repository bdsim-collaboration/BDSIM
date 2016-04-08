#include "Config.hh"
#include "Event.hh"

#include "TChain.h"

#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventModel.hh"



class DataLoader
{
public :
  DataLoader();
  void CommonCtor();
  void BuildInputFileList();
  void BuildTreeNameList();
  void BuildSamplerNameList();
  void ChainTrees();
  virtual ~DataLoader();
  BDSOutputROOTEventOptions*  GetOptions();
  BDSOutputROOTEventModel*    GetModel();
  Event*                      GetEvent();
  
private:
  BDSOutputROOTEventOptions    *opt;
  BDSOutputROOTEventModel      *mod;
  Event                        *evt;
  std::vector<std::string>      fileNames;
  
  std::vector<std::string>      samplerNames;
  std::map<std::string, int>    samplerNameMap;

  TChain *optChain;
  TChain *modChain;
  TChain *evtChain;

  ClassDef(DataLoader,1);
};
