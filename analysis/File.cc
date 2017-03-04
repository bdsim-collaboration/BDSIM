//
// Created by Stewart Boogert on 02/03/2017.
//


#include <iostream>

#include "File.hh"

#include "TFile.h"
#include "TTree.h"

#include "BDSOutputROOTEventTrajectory.hh"

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

File::File(std::vector<std::string> /*filenames*/)
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

  optionsTree->GetEntry(0);
  modelTree->GetEntry(0);
  runTree->GetEntry(0);
  eventTree->GetEntry(0);
}

void File::GetEntry(int iEntry)
{
  eventTree->GetEntry(iEntry);
}

void File::Analysis()
{
  for(int i=0;i<eventTree->GetEntries();++i) {
    this->GetEntry(i);
    std::cout << i << " " << event->trajectory->n << std::endl;

  }
}

File::~File()
{
}