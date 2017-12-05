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
#include "BDSDebug.hh"
#include "BDSOutputROOTEventHistograms.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventTrajectory.hh"
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
  Analysis("Event.", nullptr, "EventHistogramsMerged"),
  event(nullptr),
  printModulo(1),
  processSamplers(false),
  emittanceOnTheFly(false)
{;}

EventAnalysis::EventAnalysis(Event*  eventIn,
			     TChain* chainIn,
			     bool    processSamplersIn,
			     bool    debugIn,
			     double  printModuloFraction,
			     bool    emittanceOnTheFlyIn):
  Analysis("Event.", chainIn, "EventHistogramsMerged", debugIn),
  event(eventIn),
  processSamplers(processSamplersIn),
  emittanceOnTheFly(emittanceOnTheFlyIn)
{
  if (processSamplers)
    {// Create sampler analyses if needed
      // Analyse the primary sampler in the optics too.
      SamplerAnalysis* sa = new SamplerAnalysis(event->GetPrimaries());
      samplerAnalyses.push_back(sa);
      
      for(auto i = event->samplers.begin(); i != event->samplers.end(); ++i)
	{
	  sa = new SamplerAnalysis(*i);
	  this->samplerAnalyses.push_back(sa);
	}
    }
  
  SetPrintModuloFraction(printModuloFraction);
}

void EventAnalysis::SetPrintModuloFraction(double fraction)
{
  int    nEntries = chain->GetEntries();
  printModulo = (int)ceil(nEntries * fraction);
  if (printModulo <= 0)
    {printModulo = 1;}
}

EventAnalysis::~EventAnalysis()
{
  for (auto& sa : samplerAnalyses)
    {delete sa;}
}

void EventAnalysis::Process()
{
  if (debug)
    {std::cout << __METHOD_NAME__ << std::endl;}
  Initialise();

  if(debug)
    {std::cout << __METHOD_NAME__ << "Entries: " << chain->GetEntries() << " " << std::endl;}

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
	{histoSum = new HistogramMerge(event->histos, debug);}
      else
	{histoSum->Add(event->histos);}

      UserProcess();

      if(debug)
	{
	  std::cout << __METHOD_NAME__ << i << std::endl;
        if (processSamplers)
        {
            std::cout << __METHOD_NAME__ << "Vector lengths" << std::endl;
            std::cout << __METHOD_NAME__ << "primaries=" << this->event->primaries->n << std::endl;
            std::cout << __METHOD_NAME__ << "eloss=" << this->event->eloss->n << std::endl;
            std::cout << __METHOD_NAME__ << "nprimary=" << this->event->primaryFirstHit->n << std::endl;
            std::cout << __METHOD_NAME__ << "nlast=" << this->event->primaryLastHit->n << std::endl;
            std::cout << __METHOD_NAME__ << "ntunnel=" << this->event->tunnelHit->n << std::endl;
            std::cout << __METHOD_NAME__ << "ntrajectory=" << this->event->trajectory->n << std::endl;
            //      std::cout << "EventAnalysis::Process> " << this->event->sampler->samplerName << std::endl;
        }
	}
      
      if(processSamplers)
	{ProcessSamplers(i==0);}
    }
  std::cout << "\rSampler analysis complete                           " << std::endl;
}

void EventAnalysis::Terminate()
{
  Analysis::Terminate();

  if (processSamplers)
    {
      //vector of emittance values and errors: emitt_x, emitt_y, err_emitt_x, err_emitt_y
      std::vector<double> emittance = {0,0,0,0};
      for (auto samplerAnalysis : samplerAnalyses)
	{
	  emittance = samplerAnalysis->Terminate(emittance, !emittanceOnTheFly);
	  opticalFunctions.push_back(samplerAnalysis->GetOpticalFunctions());
	}
    }
}

void EventAnalysis::Write(TFile *outputFile)
{
  if(debug)
    {std::cout << __METHOD_NAME__ << std::endl;}

  // Write rebdsim histograms:
  Analysis::Write(outputFile);

  // We don't need to write out the optics tree if we didn't process samplers
  // as there's no possibility of optical data.
  if (!processSamplers)
    {return;}

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

  opticsTree->Branch("Sigma_Emitt_x", &(xOpticsPoint[12]), "Sigma_Emitt_x/D");
  opticsTree->Branch("Sigma_Emitt_y", &(yOpticsPoint[12]), "Sigma_Emitt_y/D");
  opticsTree->Branch("Sigma_Alpha_x", &(xOpticsPoint[13]), "Sigma_Alpha_x/D");
  opticsTree->Branch("Sigma_Alpha_y", &(yOpticsPoint[13]), "Sigma_Alpha_y/D");
  opticsTree->Branch("Sigma_Beta_x",  &(xOpticsPoint[14]), "Sigma_Beta_x/D");
  opticsTree->Branch("Sigma_Beta_y",  &(yOpticsPoint[14]), "Sigma_Beta_y/D");
  opticsTree->Branch("Sigma_Gamma_x", &(xOpticsPoint[15]), "Sigma_Gamma_x/D");
  opticsTree->Branch("Sigma_Gamma_y", &(yOpticsPoint[15]), "Sigma_Gamma_y/D");
  opticsTree->Branch("Sigma_Disp_x",  &(xOpticsPoint[16]), "Sigma_Disp_x/D");
  opticsTree->Branch("Sigma_Disp_y",  &(yOpticsPoint[16]), "Sigma_Disp_y/D");
  opticsTree->Branch("Sigma_Disp_xp", &(xOpticsPoint[17]), "Sigma_Disp_xp/D");
  opticsTree->Branch("Sigma_Disp_yp", &(yOpticsPoint[17]), "Sigma_Disp_yp/D");
  opticsTree->Branch("Sigma_Mean_x",  &(xOpticsPoint[18]), "Sigma_Mean_x/D");
  opticsTree->Branch("Sigma_Mean_y",  &(yOpticsPoint[18]), "Sigma_Mean_y/D");
  opticsTree->Branch("Sigma_Mean_xp", &(xOpticsPoint[19]), "Sigma_Mean_xp/D");
  opticsTree->Branch("Sigma_Mean_yp", &(yOpticsPoint[19]), "Sigma_Mean_yp/D");
  opticsTree->Branch("Sigma_Sigma_x", &(xOpticsPoint[20]), "Sigma_Sigma_x/D");
  opticsTree->Branch("Sigma_Sigma_y", &(yOpticsPoint[20]), "Sigma_Sigma_y/D");
  opticsTree->Branch("Sigma_Sigma_xp",&(xOpticsPoint[21]), "Sigma_Sigma_xp/D");
  opticsTree->Branch("Sigma_Sigma_yp",&(yOpticsPoint[21]), "Sigma_Sigma_yp/D");

  opticsTree->Branch("Mean_E",        &(lOpticsPoint[6]),  "Mean_E/D");
  opticsTree->Branch("Mean_t",        &(lOpticsPoint[7]),  "Mean_t/D");
  opticsTree->Branch("Sigma_E",       &(lOpticsPoint[8]),  "Sigma_E/D");
  opticsTree->Branch("Sigma_t",       &(lOpticsPoint[9]),  "Sigma_t/D");
  opticsTree->Branch("Sigma_Mean_E",  &(lOpticsPoint[18]), "Sigma_Mean_E/D");
  opticsTree->Branch("Sigma_Mean_t",  &(lOpticsPoint[19]), "Sigma_Mean_t/D");
  opticsTree->Branch("Sigma_Sigma_E", &(lOpticsPoint[20]), "Sigma_Sigma_E/D");
  opticsTree->Branch("Sigma_Sigma_t", &(lOpticsPoint[21]), "Sigma_Sigma_t/D");


  for(const auto entry : opticalFunctions)
    {
      xOpticsPoint = entry[0];
      yOpticsPoint = entry[1];
      lOpticsPoint = entry[2];
      opticsTree->Fill();
    }
  opticsTree->Write();
}

void EventAnalysis::ProcessSamplers(bool firstTime)
{
  if (debug)
    {std::cout << __METHOD_NAME__ << std::endl;}
  if (processSamplers)
    {
      if (debug)
	{std::cout << __METHOD_NAME__ << std::endl;}
      for(auto s : samplerAnalyses)
	{s->Process(firstTime);}
    }
}

void EventAnalysis::Initialise()
{
  if (debug)
    {std::cout << __METHOD_NAME__ << std::endl;}
  if (processSamplers)
    {
      if (debug)
	{std::cout << __METHOD_NAME__ << std::endl;}
      for (auto s : samplerAnalyses)
	{s->Initialise();}
    }
}
