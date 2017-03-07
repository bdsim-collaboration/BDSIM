//
// Created by Stewart Boogert on 02/03/2017.
//

#ifndef BDSIM_ANALYSISUSER_H
#define BDSIM_ANALYSISUSER_H

#include <memory>
#include <string>
#include <vector>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

#include "Options.hh"
#include "Model.hh"
#include "Run.hh"
#include "Event.hh"

/**
 * @brief User analysis class
 *
 * @author Stewart Boogert
 */

class AnalysisUser {
public:
  AnalysisUser();
  AnalysisUser(std::string filename);
  AnalysisUser(std::vector<std::string> filenames);
  void MakeBranches(std::shared_ptr< TTree > outputTree);
  void SetBranchAddresses(std::shared_ptr< TTree > options, std::shared_ptr< TTree > model, std::shared_ptr< TTree > run, std::shared_ptr< TTree > event);
  unsigned int GetNumSamplers();
  void GetEntry(int iEntry);
  void Analysis();
  virtual ~AnalysisUser();

  //smart pointers?
  std::shared_ptr< TFile > f;
  std::shared_ptr< TTree > optionsTree;
  std::shared_ptr< TTree > modelTree;
  std::shared_ptr< TTree > runTree;
  std::shared_ptr< TTree > eventTree;
  std::shared_ptr< TFile > foutput;
  std::vector< std::shared_ptr<TTree> > outputTree_Samplers;

  std::shared_ptr< Options > options;
  std::shared_ptr< Model   > model;
  std::shared_ptr< Event   > event;
  std::shared_ptr< Run     > run;

  char vertexmodel[10]{'0','0','0','0','0','0','0','0','0','\0'};
  float vertexx{999};
  float vertexy{999};
  float vertexz{999};
  int vertexprocess{999};
  int vertexsubprocess{999};
  int trackID{999};
  int PDG{999};
  int parentID{999};
private:
  ClassDef(AnalysisUser,1);
  unsigned int number_samplers{999};
};


#endif //BDSIM_FILE_H
