#include "EventAnalysisOrbit.hh"
#include "SamplerAnalysis.hh"

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"

void EventAnalysisOrbit::Clear()
{
  ss.clear();
  x.clear();
  xp.clear();
  y.clear();
  yp.clear();
}

void EventAnalysisOrbit::ExtractOrbit(int index)
{
  const int entries = chain->GetEntries();
  if (index > entries-1)
    {
      std::string errString = "Orbit index: " + std::to_string(index) +
	" greater than number of events: " + std::to_string(entries) + " in file minus one";
      throw errString;
    }

  int nSamplers = (int)samplerAnalyses.size();

  std::cout << "Getting orbit " << index << std::endl;
  chain->GetEntry(index);
  std::cout << "Loaded" << std::endl;
  
  int counter = 0;
  for(auto s : samplerAnalyses)
    {
      std::cout << "\rSampler #" << std::setw(6) << counter << " of " << nSamplers;
      std::cout.flush();
      if (s->s->n > 0)
	{// valid entry on that sampler
	  ss.push_back(s->s->S);
	  x.push_back(s->s->x[0]);
	  xp.push_back(s->s->xp[0]);
	  y.push_back(s->s->y[0]);
	  yp.push_back(s->s->yp[0]);
	}
      counter++;
    }
  std::cout << std::endl;
}

void EventAnalysisOrbit::WriteOrbit(TFile* f)
{
  f->cd("/");
  TTree* orbitTree = new TTree("orbit", "orbit");

  double dss;
  double dx;
  double dxp;
  double dy;
  double dyp;

  orbitTree->Branch("s",  &dss, "s/D");
  orbitTree->Branch("x",  &dx,  "x/D");
  orbitTree->Branch("xp", &dxp, "xp/D");
  orbitTree->Branch("y",  &dy,  "y/D");
  orbitTree->Branch("yp", &dyp, "yp/D");
  
  for (int i = 0; i < (int)ss.size(); ++i)
    {
      dss = ss[i];
      dx  = x[i];
      dxp = xp[i];
      dy  = y[i];
      dyp = yp[i];
      orbitTree->Fill();
    }
  orbitTree->Write();
}
