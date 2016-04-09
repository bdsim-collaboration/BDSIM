#ifndef __EVENTANALYSIS_H
#define __EVENTANALYSIS_H


#include "TChain.h"

#include "Event.hh"

class EventAnalysis
{
public:
  EventAnalysis(Event *, TChain *);
  ~EventAnalysis();
  void Process();
protected:
  Event  *event;
  TChain *chain;
};

#endif