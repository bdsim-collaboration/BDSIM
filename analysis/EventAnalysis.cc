#include "EventAnalysis.hh"
#include "Config.hh"

EventAnalysis::EventAnalysis()
{
  event = nullptr;
  chain = nullptr;

}

EventAnalysis::EventAnalysis(Event *eventIn, TChain *chainIn)
{
  this->event = eventIn;
  this->chain = chainIn;
  // create sampler analyses
  for(auto i = event->samplers.begin(); i != event->samplers.end(); ++i)
  {
    this->samplerAnalyses.push_back(new SamplerAnalysis(*i));
  }
}

EventAnalysis::~EventAnalysis()
{
}

void EventAnalysis::Process()
{
  if(Config::Instance()->Debug())
  {
    std::cout << "EventAnalysis::Process> " << this->chain->GetEntries() << " " << std::endl;
  }
  // loop over events
  for(int i=0;i<this->chain->GetEntries();++i) {
    this->chain->GetEntry(i);

    if(Config::Instance()->Debug())
    {
      std::cout << "EventAnalysis::Process> Got entry : " << i << std::endl;
      std::cout << "EventAnalysis::Process> Vector lengths : "
                                              << this->event->primaries->n << " : "
                                              << this->event->eloss->n   << " : "
                                              << this->event->primaryFirstHit->n << " : "
                                              << this->event->primaryLastHit->n  << " : "
                                              << this->event->tunnelHit->n << std::endl;
//      std::cout << "EventAnalysis::Process> " << this->event->sampler->samplerName.c_str() << std::endl;
    }

    this->ProcessSamplers();

  }
}

void EventAnalysis::ProcessSamplers()
{
  for(auto s = event->samplers.begin(); s != event->samplers.end(); ++s)
  {
    if(Config::Instance()->Debug())
    {
      std::cout << "EventAnalysis::ProcessSamplers> " << (*s)->samplerName.c_str() << " " << (*s)->n <<std::endl;
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