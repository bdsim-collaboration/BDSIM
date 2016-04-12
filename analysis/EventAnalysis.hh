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


protected:
  Event  *event;
  TChain *chain;
  std::vector<SamplerAnalysis*> samplerAnalyses;
  std::map<std::string, TH1*>   histograms1D;
  std::map<std::string, TH2*>   histograms2D;
};

#endif
