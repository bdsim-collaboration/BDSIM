//
// Created by Stewart Boogert on 02/03/2017.
//

#ifndef BDSIM_FILE_H
#define BDSIM_FILE_H

#include <string>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

#include "Options.hh"
#include "Model.hh"
#include "Run.hh"
#include "Event.hh"

class File {
public:
  File();
  File(std::string filename);
  File(std::vector<std::string> filenames);
  void SetBranchAddresses(TTree *options, TTree *model, TTree *run, TTree *event);
  virtual ~File();

  TFile *f;
  TTree *optionsTree;
  TTree *modelTree;
  TTree *runTree;
  TTree *eventTree;

  Options *options;
  Model   *model;
  Event   *event;
  Run     *run;

private:
  ClassDef(File,1);
};


#endif //BDSIM_FILE_H
