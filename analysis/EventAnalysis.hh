#ifndef __EVENTANALYSIS_H
#define __EVENTANALYSIS_H

#include <vector>
#include <map>

#include "TChain.h"
#include "TH1D.h"
#include "TH2D.h"

#include "Event.hh"
#include "SamplerAnalysis.hh"

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

  void Write(std::string outputFileName);

protected:
  Event  *event;
  TChain *chain;
  std::vector<SamplerAnalysis*> samplerAnalyses;
  std::vector<std::string>      histogramNames;
  std::map<std::string, TH1*>   histograms1D;
  std::map<std::string, TH2*>   histograms2D;
  BDSOutputROOTEventHistograms  *histoSum;

  ClassDef(EventAnalysis,1);
};

#endif
