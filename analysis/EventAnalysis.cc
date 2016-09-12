#include "BDSDebug.hh"
#include "BDSOutputROOTEventHistograms.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventTrajectory.hh"
#include "Config.hh"
#include "Event.hh"
#include "EventAnalysis.hh"
#include "HistogramMerge.hh"
#include "SamplerAnalysis.hh"

#include "TROOT.h"
#include "TChain.h"
#include "TDirectory.h"
#include "TFile.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

ClassImp(EventAnalysis)

EventAnalysis::EventAnalysis():
Analysis("Event.", nullptr, "bdsimEventMergedHistograms"),
  event(nullptr)
{;}

EventAnalysis::EventAnalysis(Event *eventIn, TChain* chain, bool debug):
  Analysis("Event.", chain, "bdsimEventMergedHistograms", debug),
  event(eventIn)
{
  // create sampler analyses
  for(auto i = event->samplers.begin(); i != event->samplers.end(); ++i)
  {
    SamplerAnalysis *sa = new SamplerAnalysis(*i);
    this->samplerAnalyses.push_back(sa);
  }

  double fraction = Config::Instance()->PrintModuloFraction();
  int    nEntries = chain->GetEntries();
  printModulo = (int)ceil(nEntries*fraction);
  if (printModulo < 0)
    {printModulo = 1;}
}

EventAnalysis::~EventAnalysis()
{;}

void EventAnalysis::Process()
{
  Initialise();

  if(debug)
    {std::cout << __METHOD_NAME__ << this->chain->GetEntries() << " " << std::endl;}

  // loop over events
  const int entries = chain->GetEntries();
  for(int i=0; i<entries; ++i)
    {
      this->chain->GetEntry(i);
      // event analysis feedback
      if (i % printModulo == 0)
	{
	  std::cout << "\rEvent #" << std::setw(6) << i << " of " << entries;
	  if (!debug)
	    {std::cout.flush();}
	  else
	    {std::cout << std::endl;}
	}
      
      if(i==0)
	{histoSum = new HistogramMerge(event->histos);}
      else
	{histoSum->Add(event->histos);}

      if(debug)
	{
	  std::cout << __METHOD_NAME__ << i << std::endl;
	  std::cout << __METHOD_NAME__ << "Vector lengths" << std::endl;
	  std::cout << __METHOD_NAME__ << "primaries=" << this->event->primaries->n << std::endl;
	  std::cout << __METHOD_NAME__ << "eloss=" << this->event->eloss->n << std::endl;
	  std::cout << __METHOD_NAME__ << "nprimary=" << this->event->primaryFirstHit->n << std::endl;
	  std::cout << __METHOD_NAME__ << "nlast=" << this->event->primaryLastHit->n << std::endl;
	  std::cout << __METHOD_NAME__ << "ntunnel=" << this->event->tunnelHit->n << std::endl;
	  std::cout << __METHOD_NAME__ << "ntrajectory=" << this->event->trajectory->n << std::endl;
	  //      std::cout << "EventAnalysis::Process> " << this->event->sampler->samplerName << std::endl;
	}
      
      if(Config::Instance()->ProcessSamplers())
	{ProcessSamplers();}
    }
  std::cout << "\rComplete                                       " << std::endl;
}

void EventAnalysis::Terminate()
{
  Analysis::Terminate();

  for(auto i = this->samplerAnalyses.begin(); i != this->samplerAnalyses.end(); ++i)
  {
    (*i)->Terminate();
    this->opticalFunctions.push_back((*i)->GetOpticalFunctions());
  }
}

void EventAnalysis::Write(TFile *outputFile)
{
  if(debug)
    {std::cout << __METHOD_NAME__ << std::endl;}

  //Write rebdsim histograms:
  Analysis::Write(outputFile);

  outputFile->cd("/");

  std::vector<double> xOpticsPoint;
  std::vector<double> yOpticsPoint;
  std::vector<double> lOpticsPoint;
  xOpticsPoint.resize(24);
  yOpticsPoint.resize(24);
  lOpticsPoint.resize(24);

  // write optical functions
  TTree *opticsTree = new TTree("optics","optics");
  opticsTree->Branch("Emitt_x", &(xOpticsPoint[0]), "Emitt_x/D");
  opticsTree->Branch("Emitt_y", &(yOpticsPoint[0]), "Emitt_y/D");
  opticsTree->Branch("Alpha_x", &(xOpticsPoint[1]), "Alpha_x/D");
  opticsTree->Branch("Alpha_y", &(yOpticsPoint[1]), "Alpha_y/D");
  opticsTree->Branch("Beta_x",  &(xOpticsPoint[2]), "Beta_x/D");
  opticsTree->Branch("Beta_y",  &(yOpticsPoint[2]), "Beta_y/D");
  opticsTree->Branch("Gamma_x", &(xOpticsPoint[3]), "Gamma_x/D");
  opticsTree->Branch("Gamma_y", &(yOpticsPoint[3]), "Gamma_y/D");
  opticsTree->Branch("Disp_x",  &(xOpticsPoint[4]), "Disp_x/D");
  opticsTree->Branch("Disp_y",  &(yOpticsPoint[4]), "Disp_y/D");
  opticsTree->Branch("Disp_xp", &(xOpticsPoint[5]), "Disp_xp/D");
  opticsTree->Branch("Disp_yp", &(yOpticsPoint[5]), "Disp_yp/D");
  opticsTree->Branch("Mean_x",  &(xOpticsPoint[6]), "Mean_x/D");
  opticsTree->Branch("Mean_y",  &(yOpticsPoint[6]), "Mean_y/D");
  opticsTree->Branch("Mean_xp", &(xOpticsPoint[7]), "Mean_xp/D");
  opticsTree->Branch("Mean_yp", &(yOpticsPoint[7]), "Mean_yp/D");
  opticsTree->Branch("Sigma_x", &(xOpticsPoint[8]), "Sigma_x/D");
  opticsTree->Branch("Sigma_y", &(yOpticsPoint[8]), "Sigma_y/D");
  opticsTree->Branch("Sigma_xp",&(xOpticsPoint[9]), "Sigma_xp/D");
  opticsTree->Branch("Sigma_yp",&(yOpticsPoint[9]), "Sigma_yp/D");
  opticsTree->Branch("S"       ,&(xOpticsPoint[10]),"S/D");
  opticsTree->Branch("Npart"   ,&(xOpticsPoint[11]),"Npart/D");

  opticsTree->Branch("Sig_Emitt_x", &(xOpticsPoint[12]), "Sig_Emitt_x/D");
  opticsTree->Branch("Sig_Emitt_y", &(yOpticsPoint[12]), "Sig_Emitt_y/D");
  opticsTree->Branch("Sig_Alpha_x", &(xOpticsPoint[13]), "Sig_Alpha_x/D");
  opticsTree->Branch("Sig_Alpha_y", &(yOpticsPoint[13]), "Sig_Alpha_y/D");
  opticsTree->Branch("Sig_Beta_x",  &(xOpticsPoint[14]), "Sig_Beta_x/D");
  opticsTree->Branch("Sig_Beta_y",  &(yOpticsPoint[14]), "Sig_Beta_y/D");
  opticsTree->Branch("Sig_Gamma_x", &(xOpticsPoint[15]), "Sig_Gamma_x/D");
  opticsTree->Branch("Sig_Gamma_y", &(yOpticsPoint[15]), "Sig_Gamma_y/D");
  opticsTree->Branch("Sig_Disp_x",  &(xOpticsPoint[16]), "Sig_Disp_x/D");
  opticsTree->Branch("Sig_Disp_y",  &(yOpticsPoint[16]), "Sig_Disp_y/D");
  opticsTree->Branch("Sig_Disp_xp", &(xOpticsPoint[17]), "Sig_Disp_xp/D");
  opticsTree->Branch("Sig_Disp_yp", &(yOpticsPoint[17]), "Sig_Disp_yp/D");
  opticsTree->Branch("Sig_Mean_x",  &(xOpticsPoint[18]), "Sig_Mean_x/D");
  opticsTree->Branch("Sig_Mean_y",  &(yOpticsPoint[18]), "Sig_Mean_y/D");
  opticsTree->Branch("Sig_Mean_xp", &(xOpticsPoint[19]), "Sig_Mean_xp/D");
  opticsTree->Branch("Sig_Mean_yp", &(yOpticsPoint[19]), "Sig_Mean_yp/D");
  opticsTree->Branch("Sig_Sigma_x", &(xOpticsPoint[20]), "Sig_Sigma_x/D");
  opticsTree->Branch("Sig_Sigma_y", &(yOpticsPoint[20]), "Sig_Sigma_y/D");
  opticsTree->Branch("Sig_Sigma_xp",&(xOpticsPoint[21]), "Sig_Sigma_xp/D");
  opticsTree->Branch("Sig_Sigma_yp",&(yOpticsPoint[21]), "Sig_Sigma_yp/D");

  opticsTree->Branch("Mean_E",      &(lOpticsPoint[6]), "Mean_E/D");
  opticsTree->Branch("Mean_t",      &(lOpticsPoint[7]), "Mean_t/D");
  opticsTree->Branch("Sigma_E",     &(lOpticsPoint[8]), "Sigma_E/D");
  opticsTree->Branch("Sigma_t",     &(lOpticsPoint[9]), "Sigma_t/D");
  opticsTree->Branch("Sig_Mean_E",  &(lOpticsPoint[18]), "Sig_Mean_E/D");
  opticsTree->Branch("Sig_Mean_t",  &(lOpticsPoint[19]), "Sig_Mean_t/D");
  opticsTree->Branch("Sig_Sigma_E", &(lOpticsPoint[20]), "Sig_Sigma_E/D");
  opticsTree->Branch("Sig_Sigma_t", &(lOpticsPoint[21]), "Sig_Sigma_t/D");


  for(auto i : this->opticalFunctions)
  {
    xOpticsPoint = i[0];
    yOpticsPoint = i[1];
    lOpticsPoint = i[2];
    opticsTree->Fill();
  }
  opticsTree->Write();

}

void EventAnalysis::ProcessSamplers()
{
  for(auto s = this->samplerAnalyses.begin(); s != this->samplerAnalyses.end(); ++s)
  {
    if(debug)
    {
      std::cout << "EventAnalysis::ProcessSamplers> " << (*s)->s->samplerName << " " << (*s)->s->n <<std::endl;
    }

    // process samplers
    (*s)->Process();
  }

}

void EventAnalysis::Initialise()
{
  for(auto i = this->samplerAnalyses.begin(); i != this->samplerAnalyses.end(); ++i)
    {(*i)->Initialise();}
}
