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

void StringToCharArray(std::string const component_name, char * vertexmodel){
  std::vector< char > tempvec(component_name.begin(),component_name.end());
  tempvec.push_back('\0');
  strcpy(vertexmodel,&tempvec[0]);
}

ClassImp(AnalysisUser)

AnalysisUser::AnalysisUser(){}

AnalysisUser::AnalysisUser(std::string const filename)
{
  gROOT->Reset();
  std::shared_ptr< TFile > tmpfile(new TFile(filename.data()));
  f           = tmpfile;
  std::shared_ptr< TTree > optionsTreeTemp(dynamic_cast< TTree* >(f->Get("Options")));
  optionsTree = optionsTreeTemp;
  std::shared_ptr< TTree > modelTreeTemp(dynamic_cast< TTree* > (f->Get("Model")));
  modelTree   = modelTreeTemp;
  std::shared_ptr< TTree > runTreeTemp(dynamic_cast< TTree* > (f->Get("Run")));
  runTree     = runTreeTemp;
  std::shared_ptr< TTree > eventTreeTemp(dynamic_cast< TTree* > (f->Get("Event")));
  eventTree   = eventTreeTemp;

  std::shared_ptr< Options > optionstemp (new Options());
  options = optionstemp;
  std::shared_ptr< Model >   modeltemp   (new Model());
  model   = modeltemp;
  std::shared_ptr< Run >     runtemp     (new Run());
  run     = runtemp;
  std::shared_ptr< Event>    eventtemp   (new Event(false,true));
  event   = eventtemp;

  this->SetBranchAddresses(optionsTree,modelTree,runTree,eventTree);
  number_samplers = GetNumSamplers();

  std::string const outputfilename = "AnalysisUserOutput_" + filename ;
  std::shared_ptr< TFile > tmpoutputfile(new TFile(outputfilename.data(),"CREATE","Output ROOT file from BDSIM AnalysisUser")); 
  foutput = tmpoutputfile;
  
  for(size_t s = 0; s < number_samplers; ++s)                       // Loop over samplers
  {
    std::stringstream ss;
    ss << "Sampler" << s;
    std::shared_ptr< TTree > TreeTemp( new TTree(("AnalysisUser_"+ss.str()).c_str(),("AnalysisUser TTree containing vertex information of the particles recorded in "+ss.str()).c_str()) ) ;
    outputTree_Samplers.push_back( TreeTemp );
    MakeBranches(outputTree_Samplers.at(s));
  }
}

AnalysisUser::AnalysisUser(std::vector<std::string> filenames){}

AnalysisUser::~AnalysisUser(){}

void AnalysisUser::SetBranchAddresses(std::shared_ptr< TTree > optionsTree, std::shared_ptr< TTree > modelTree, std::shared_ptr< TTree > runTree, std::shared_ptr< TTree > eventTree)
{
  options->SetBranchAddress(optionsTree.get());
  model->SetBranchAddress(modelTree.get());
  run->SetBranchAddress(runTree.get());

  optionsTree->GetEntry(0);
  modelTree->GetEntry(0);
  runTree->GetEntry(0);
  eventTree->GetEntry(0);

  event->SetBranchAddress(eventTree.get(),&(model->model->samplerNamesUnique));
}

void AnalysisUser::MakeBranches(std::shared_ptr< TTree > outputTree)
{
  outputTree->Branch("VertexModel",(void*)&vertexmodel[0],"VertexModel[10]/C");
  outputTree->Branch("VertexX",&vertexx,"VertexX/F");
  outputTree->Branch("VertexY",&vertexy,"VertexY/F");
  outputTree->Branch("VertexZ",&vertexz,"VertexZ/F");
  outputTree->Branch("VertexProcess",&vertexprocess,"VertexProcess/I");
  outputTree->Branch("VertexSubProcess",&vertexsubprocess,"VertexSubProcess/I");
  outputTree->Branch("TrackID",&trackID,"TrackID/I");
  outputTree->Branch("PDGID",&PDG,"PDGID/I");
  outputTree->Branch("ParentID",&parentID,"ParentID/I");
}

unsigned int AnalysisUser::GetNumSamplers()
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
  for(long long int i = 0; i < eventTree->GetEntries(); ++i) {      // Loop over file entries
    this->GetEntry(i);                                              // Get entry
    for(size_t s = 0; s < number_samplers; ++s)                      // Loop over samplers
    {
      for(int j = 0; j < this->event->samplers[s]->n; ++j)            // Loop over sampler hits in sampler 0
      {
        int trackid = this->event->samplers[s]->trackID[j];         // track ID for sampler info
        if(trackid != 1) {                                          // does not work for the primary
          BDSOutputROOTEventTrajectoryPoint point =
            event->trajectory->primaryProcessPoint(trackid);        // get initial process point
          StringToCharArray(model->model->componentName[point.model],vertexmodel);
          vertexx = point.position.X();
          vertexy = point.position.Y();
          vertexz = point.position.Z();
          vertexprocess = point.processType;
          vertexsubprocess = point.processSubType;
        }else
        {
          StringToCharArray("Primary",vertexmodel);
          vertexx = 0;
          vertexy = 0;
          vertexz = 0;
          vertexprocess = -1;
          vertexsubprocess = -1;
        }
        trackID = trackid;
        PDG = event->samplers[s]->partID[j]; 
        parentID = event->samplers[s]->parentID[j]; 
        outputTree_Samplers.at(s)->Fill();
      }      
    }//End of samplers loop
  }
  foutput->Write();
  foutput->Close();
}
