//
// Created by Stewart Boogert on 02/03/2017.
//


#include <iostream>
#include <include/BDSOutputROOTEventModel.hh>

#include "AnalysisUser.hh"

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

#include "BDSOutputROOTEventTrajectory.hh"

ClassImp(AnalysisUser)

AnalysisUser::AnalysisUser()
{
}

AnalysisUser::AnalysisUser(std::string filename)
{
  gROOT->Reset();
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
  number_samplers = GetNumSamplers();

  std::string outputfilename = "AnalysisUserOutput_" + filename ;
  foutput = new TFile(outputfilename.data(),"CREATE","Output ROOT file from BDSIM AnalysisUser"); 
  
  for(size_t s = 0; s<number_samplers; ++s)                       // Loop over samplers
  {
    std::stringstream ss;
    ss<< "Sampler" << s;
    outputTree_Samplers.emplace_back( new TTree(("AnalysisUser_"+ss.str()).c_str(),("AnalysisUser TTree containing vertex information of the particles recorded in "+ss.str()).c_str()) );
    MakeBranches(outputTree_Samplers.at(s));
  }
}

AnalysisUser::AnalysisUser(std::vector<std::string> filenames)
{

}


AnalysisUser::~AnalysisUser()
{
  std::cout << "Destructor is called." << std::endl;
  delete f;
  delete optionsTree;
  delete modelTree;
  delete runTree;
  delete eventTree;

  delete options;
  delete model;
  delete run;
  delete event;

  delete foutput;
  for(size_t s = 0; outputTree_Samplers.size(); ++s){
    delete outputTree_Samplers.at(s);
  }
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

void AnalysisUser::MakeBranches(TTree *outputTree)
{
  outputTree->Branch("VertexModel",(void*)&vertexmodel[0],"VertexModel[10]/C");
  outputTree->Branch("VertexX",&vertexx,"VertexX/F");
  outputTree->Branch("VertexY",&vertexy,"VertexY/F");
  outputTree->Branch("VertexZ",&vertexz,"VertexZ/F");
  outputTree->Branch("VertexProcess",&vertexprocess,"VertexProcess/I");
  outputTree->Branch("VertexSubProcess",&vertexsubprocess,"VertexSubProcess/I");
}

int AnalysisUser::GetNumSamplers()
{
  eventTree->GetEntry(0);
  return this->event->samplers.size();
}

void AnalysisUser::GetEntry(int iEntry)
{
  eventTree->GetEntry(iEntry);
}

void AnalysisUser::Analysis()
{
  std::cout << "Saving vertex information to ROOT output file." << std::endl; 
  for(int i=0;i<eventTree->GetEntries();++i) {                      // Loop over file entries
    this->GetEntry(i);                                              // Get entry

    for(size_t s = 0; s< number_samplers; ++s)         // Loop over samplers
    {
      for(int j = 0; j<this->event->samplers[s]->n; ++j)            // Loop over sampler hits in sampler 0
      {
        int trackID = this->event->samplers[s]->trackID[j];         // track ID for sampler info
        if(trackID != 1) {                                          // does not work for the primary
          BDSOutputROOTEventTrajectoryPoint point =
            event->trajectory->primaryProcessPoint(trackID);    // get initial process point
          std::string temp = model->model->componentName[point.model];
          std::vector< char > tempvec(temp.begin(),temp.end());
          tempvec.push_back('\0');
          strcpy(vertexmodel,&tempvec[0]);
          vertexx = point.position.X();
          vertexy = point.position.Y();
          vertexz = point.position.Z();
          vertexprocess = point.processType;
          vertexsubprocess = point.processSubType;

          outputTree_Samplers.at(s)->Fill();

        }
      }
    }//End of samplers loop
  }
  foutput->Write();
  foutput->Close();
}
