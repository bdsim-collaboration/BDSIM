#ifndef __EVENTANALYSIS_H
#define __EVENTANALYSIS_H


#include "TChain.h"

#include "Event.hh"
#include "SamplerAnalysis.hh"

class EventAnalysis
{
public:
  EventAnalysis();
  EventAnalysis(Event *, TChain *);
  virtual ~EventAnalysis();
  void Process();
  void ProcessSamplers();
  void SimpleHistograms();
protected:
  Event  *event;
  TChain *chain;
  std::vector<SamplerAnalysis*> samplerAnalyses;
};

#endif