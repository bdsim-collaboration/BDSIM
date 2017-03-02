//
// Created by Stewart Boogert on 02/03/2017.
//

#include "File.hh"

#include "TFile.h"
#include "TTree.h"

ClassImp(File)

File::File()
{
}

File::File(std::string filename)
{
  f           = new TFile(filename.data());
  optionsTree = (TTree*)f->Get("Options");
  modelTree   = (TTree*)f->Get("Model");
  runTree     = (TTree*)f->Get("Run");
  eventTree   = (TTree*)f->Get("Event");

  options = new Options();
  model   = new Model();
  run     = new Run();
  event   = new Event();

  this->SetBranchAddresses(optionsTree,modelTree,runTree,eventTree);
}

File::File(std::vector<std::string> filenames)
{
  delete f;
  delete optionsTree;
  delete modelTree;
  delete runTree;
  delete eventTree;

  delete options;
  delete model;
  delete run;
  delete event;
}

void File::SetBranchAddresses(TTree *optionsTree, TTree *modelTree, TTree *runTree, TTree *eventTree)
{
  options->SetBranchAddress(optionsTree);
  model->SetBranchAddress(modelTree);
  run->SetBranchAddress(runTree);
  event->SetBranchAddress(eventTree);
}

File::~File()
{
}