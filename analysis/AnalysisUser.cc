#include <iostream>

#include "AnalysisUser.hh"

#include "TFile.h"
#include "TTree.h"

#include "BDSOutputROOTEventModel.hh"
#include "BDSOutputROOTEventTrajectory.hh"

ClassImp(AnalysisUser)

AnalysisUser::AnalysisUser()
{
}

AnalysisUser::AnalysisUser(std::string filename)
{
  f           = new TFile(filename.data());
  optionsTree = (TTree*)f->Get("Options");
  modelTree   = (TTree*)f->Get("Model");
  runTree     = (TTree*)f->Get("Run");
  eventTree   = (TTree*)f->Get("Event");

  options = new Options();
  model   = new Model();
  run     = new Run();
  event   = new Event(false,true);

  this->SetBranchAddresses(optionsTree,modelTree,runTree,eventTree);
}

AnalysisUser::AnalysisUser(std::vector<std::string> filenames)
{

}


AnalysisUser::~AnalysisUser()
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

void AnalysisUser::SetBranchAddresses(TTree *optionsTree, TTree *modelTree, TTree *runTree, TTree *eventTree)
{
  options->SetBranchAddress(optionsTree);
  model->SetBranchAddress(modelTree);
  run->SetBranchAddress(runTree);

  optionsTree->GetEntry(0);
  modelTree->GetEntry(0);
  runTree->GetEntry(0);
  eventTree->GetEntry(0);

  event->SetBranchAddress(eventTree,&(model->model->samplerNamesUnique));

}

void AnalysisUser::GetEntry(int iEntry)
{
  eventTree->GetEntry(iEntry);
}

void AnalysisUser::Analysis()
{
  for(int i=0;i<eventTree->GetEntries();++i) {                      // Loop over file entries
    this->GetEntry(i);                                              // Get entry

    std::cout << "New event " << std::endl;
    std::cout << "--------- " << std::endl;

    for(int j = 0; j<this->event->samplers[0]->n; ++j)              // Loop over sampler hits in sampler 0
    {
      int trackID = this->event->samplers[0]->trackID[j];           // track ID for sampler particle
      if(trackID != 1) {                                            // does not work for the primary
        BDSOutputROOTEventTrajectoryPoint point =
                event->trajectory->primaryProcessPoint(trackID);    // get initial process point
        std::cout << i << " " << j << " "
                  << trackID << " "
                  << point.processType << " "
                  << point.processSubType << " "
                  << point.position.Z() << " "
                  << point.model << " "
                  << model->model->componentName[point.model]
                  << std::endl;

      }
    }
    //
  }
}
