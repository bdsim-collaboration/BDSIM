/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
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

AnalysisUser::AnalysisUser(std::vector<std::string> /*filenames*/)
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

void AnalysisUser::SetBranchAddresses(TTree* optionsTreeIn,
				      TTree* modelTreeIn,
				      TTree* runTreeIn,
				      TTree* eventTreeIn)
{
  options->SetBranchAddress(optionsTreeIn);
  model->SetBranchAddress(modelTreeIn);
  run->SetBranchAddress(runTreeIn);

  optionsTree->GetEntry(0);
  modelTree->GetEntry(0);
  runTree->GetEntry(0);
  eventTree->GetEntry(0);

  event->SetBranchAddress(eventTreeIn, &(model->model->samplerNamesUnique));

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
