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

class AnalysisUser
{
public:
  AnalysisUser();
  AnalysisUser(std::string filename);
  AnalysisUser(std::vector<std::string> filenames);
  void SetBranchAddresses(TTree *options, TTree *model, TTree *run, TTree *event);
  void GetEntry(int iEntry);
  void Analysis();
  virtual ~AnalysisUser();

  TFile *f           = nullptr;
  TTree *optionsTree = nullptr;
  TTree *modelTree   = nullptr;
  TTree *runTree     = nullptr;
  TTree *eventTree   = nullptr;

  Options *options   = nullptr;
  Model   *model     = nullptr;
  Event   *event     = nullptr;
  Run     *run       = nullptr;

private:
  ClassDef(AnalysisUser,1);
};


#endif
