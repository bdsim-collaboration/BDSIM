#ifndef __EVENTANALYSIS_H
#define __EVENTANALYSIS_H

#include <vector>
#include <map>

#include "TChain.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"

#include "Event.hh"
#include "SamplerAnalysis.hh"
#include "HistogramMerge.hh"

class EventAnalysis
{
public:
  EventAnalysis();
  EventAnalysis(Event *, TChain *);
  virtual ~EventAnalysis();

  void Initialise();
  void Process();
  void ProcessSamplers();
  void Terminate();

  void SimpleHistograms();
  void FillHistogram(std::string treeName, std::string histoName,
                     std::string nbins,    std::string binning,
                     std::string plot,     std::string selection);

  void Write(TFile *outputFileName);

protected:
  Event  *event;
  TChain *chain;
  std::vector<SamplerAnalysis*> samplerAnalyses;
  std::vector<std::string>      histogramNames;                   // rebdsim generated histogram names
  std::map<std::string, TH1*>   histograms1D;                     // rebdsim 1d histogram
  std::map<std::string, TH2*>   histograms2D;                     // rebdsim 2d histograms
  HistogramMerge               *histoSum;                         // bdsim histograms
  std::vector<std::vector<std::vector<double>>> opticalFunctions; // optical functions from all samplers
  ClassDef(EventAnalysis,1);
};

#endif
