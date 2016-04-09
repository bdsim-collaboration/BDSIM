#include "EventAnalysis.hh"
#include "Config.hh"

EventAnalysis::EventAnalysis(Event *eventIn, TChain *chainIn)
{
  event = eventIn;
  chain = chainIn;
}

EventAnalysis::~EventAnalysis()
{
}

void EventAnalysis::Process()
{
  // loop over events
  for(int i=0;i<this->chain->GetEntries();++i) {
    this->chain->GetEntry(i);
    if(Config::Instance()->Debug())
    {
      std::cout << "EventAnalysis::Process> " << i << std::endl;
      std::cout << "EventAnalysis::Process> " << this->event->primaries->n << " : "
                                              << this->event->eloss->n << std::endl;
    }
  }
}

void EventAnalysis::SimpleHistograms()
{
  if(Config::Instance()->Debug())
  {
    std::cout << "EventAnalysis::SimpleHistograms>" << std::endl;
  }


}