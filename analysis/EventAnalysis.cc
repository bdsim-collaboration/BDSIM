#include "EventAnalysis.hh"
#include "BDSOutputROOTEventHistograms.hh"
#include "Config.hh"

ClassImp(EventAnalysis)

EventAnalysis::EventAnalysis()
{
  event    = nullptr;
  chain    = nullptr;
  histoSum = nullptr;
}

EventAnalysis::EventAnalysis(Event *eventIn, TChain *chainIn)
{

  chainIn->GetEntry(0);

  this->event = eventIn;
  this->chain = chainIn;
  // create sampler analyses
  for(auto i = event->samplers.begin(); i != event->samplers.end(); ++i)
  {
    SamplerAnalysis *sa = new SamplerAnalysis(*i);
    this->samplerAnalyses.push_back(sa);
  }

  histoSum = new BDSOutputROOTEventHistograms(*(event->histos));
  std::cout << this->event->histos->Get1DHistogram(0) << std::endl;
  std::cout << histoSum->Get1DHistogram(0) << std::endl;
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
      std::cout << __METHOD_NAME__ << i << std::endl;
      std::cout << __METHOD_NAME__ << "Vector lengths" << std::endl;
      std::cout << __METHOD_NAME__ << "primaries="   << this->event->primaries->n << std::endl;
      std::cout << __METHOD_NAME__ << "eloss="       << this->event->eloss->n   << std::endl;
      std::cout << __METHOD_NAME__ << "nprimary="    << this->event->primaryFirstHit->n << std::endl;
      std::cout << __METHOD_NAME__ << "nlast="       << this->event->primaryLastHit->n  << std::endl;
      std::cout << __METHOD_NAME__ << "ntunnel="     << this->event->tunnelHit->n << std::endl;
      std::cout << __METHOD_NAME__ << "ntrajectory=" << this->event->trajectory->n << std::endl;
//      std::cout << "EventAnalysis::Process> " << this->event->sampler->samplerName.c_str() << std::endl;
    }

    this->ProcessSamplers();

  }
}

void EventAnalysis::ProcessSamplers()
{
  for(auto s = this->samplerAnalyses.begin(); s != this->samplerAnalyses.end(); ++s)
  {
    if(Config::Instance()->Debug())
    {
      std::cout << "EventAnalysis::ProcessSamplers> " << (*s)->s->samplerName.c_str() << " " << (*s)->s->n <<std::endl;
    }

    // process samplers
    (*s)->Process();
  }

}

void EventAnalysis::Initialise()
{
  for(auto i = this->samplerAnalyses.begin(); i != this->samplerAnalyses.end(); ++i)
  {
    (*i)->Initialise();
  }
}

void EventAnalysis::Terminate()
{
  for(auto i = this->samplerAnalyses.begin(); i != this->samplerAnalyses.end(); ++i)
  {
    (*i)->Terminate();
  }
}


void EventAnalysis::SimpleHistograms()
{
  if(Config::Instance()->Debug())
  {
    std::cout << "EventAnalysis::SimpleHistograms>" << std::endl;
  }
}
