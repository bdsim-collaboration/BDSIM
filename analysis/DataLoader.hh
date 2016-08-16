#ifndef DATALOADER_H
#define DATALOADER_H

#include "Config.hh"
#include "Event.hh"
#include "Run.hh"
#include "Model.hh"
#include "Options.hh"

#include "TChain.h"

#include "BDSOutputROOTEventRunInfo.hh"


/**
 * @brief Loader for a ROOT file using classes used to generate the file.
 *
 * @author Stewart Boogert
 */

class DataLoader
{
public:
  DataLoader();
  virtual ~DataLoader();

  /// Create an instance of each class in the file to be overlaid by loading
  /// the ROOT file.
  void CommonCtor();

  /// Build up the input file list.
  void BuildInputFileList();

  /// Open the first file in the file list and map the trees in it.
  void BuildTreeNameList();

  /// Inspect the first file (leaving it open...) and build a list of samplers
  /// by checking whether they have 'Sampler' in the name.
  void BuildEventBranchNameList();

  /// Create a tree for each sampler and add all the files to it.
  void ChainTrees();

  /// Map each chain to the member instance of each storage class in this class.
  void SetBranchAddress();

  /// @{ Accessor
  std::vector<std::string>   GetTreeNames()    {return treeNames;};
  std::vector<std::string>   GetBranchNames()  {return branchNames;}
  std::vector<std::string>   GetSamplerNames() {return samplerNames;}
  Options*                   GetOptions()      {return opt;}
  Model*                     GetModel()        {return mod;}
  Event*                     GetEvent()        {return evt;}
  Run*                       GetRun()          {return run;};
  TChain*                    GetOptionsTree()  {return optChain;}
  TChain*                    GetModelTree()    {return modChain;}
  TChain*                    GetEventTree()    {return evtChain;}
  TChain*                    GetRunTree()      {return runChain;}
  /// @}

private:
  Options                      *opt;
  Model                        *mod;
  Event                        *evt;
  Run                          *run;

  std::vector<std::string>      fileNames;
  std::vector<std::string>      safeFileNames;

  std::vector<std::string>      treeNames;
  std::vector<std::string>      branchNames;  // non-sampler branch names
  std::vector<std::string>      samplerNames; // sampler branch names
  std::map<std::string, int>    samplerNameMap;

  TChain *optChain;
  TChain *modChain;
  TChain *evtChain;
  TChain *runChain;

  ClassDef(DataLoader,1);
};

#endif
