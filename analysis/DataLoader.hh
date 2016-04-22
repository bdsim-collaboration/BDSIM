#ifndef __DATALOADER_H
#define __DATALOADER_H

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
  void BuildEventBranchNameList();
  void ChainTrees();
  void SetBranchAddress();
  virtual ~DataLoader();
  std::vector<std::string>    GetTreeNames()    { return treeNames;};
  std::vector<std::string>    GetBranchNames()  { return branchNames;}
  std::vector<std::string>    GetSamplerNames() { return samplerNames;}
  BDSOutputROOTEventOptions*  GetOptions();
  BDSOutputROOTEventModel*    GetModel();
  Event*                      GetEvent();
  TChain*                     GetOptionsTree()  { return optChain;}
  TChain*                     GetModelTree()    { return modChain;}
  TChain*                     GetEventTree()    { return evtChain;}

private:
  BDSOutputROOTEventOptions    *opt;
  BDSOutputROOTEventModel      *mod;
  Event                        *evt;

  std::vector<std::string>      fileNames;

  std::vector<std::string>      treeNames;
  std::vector<std::string>      branchNames;  // non-sampler branch names
  std::vector<std::string>      samplerNames; // sampler branch names
  std::map<std::string, int>    samplerNameMap;

  TChain *optChain;
  TChain *modChain;
  TChain *evtChain;

  ClassDef(DataLoader,1);
};

#endif
