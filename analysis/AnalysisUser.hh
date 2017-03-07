//
// Created by Stewart Boogert on 02/03/2017.
//

#ifndef BDSIM_ANALYSISUSER_H
#define BDSIM_ANALYSISUSER_H

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
  void MakeBranches(TTree *outputTree);
  void SetBranchAddresses(TTree *options, TTree *model, TTree *run, TTree *event);
  void GetEntry(int iEntry);
  void Analysis();
  virtual ~AnalysisUser();

  TFile *f;
  TTree *optionsTree;
  TTree *modelTree;
  TTree *runTree;
  TTree *eventTree;
  TFile *foutput;
  std::vector< TTree* > outputTree_Samplers;

  Options *options;
  Model   *model;
  Event   *event;
  Run     *run;

  char vertexmodel[10];
  float vertexx;
  float vertexy;
  float vertexz;
  int vertexprocess;
  int vertexsubprocess;
private:
  ClassDef(AnalysisUser,1);
};


#endif //BDSIM_FILE_H
