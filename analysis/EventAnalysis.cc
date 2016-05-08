#include "EventAnalysis.hh"
#include "BDSOutputROOTEventHistograms.hh"
#include "Config.hh"
#include "TROOT.h"
#include "TDirectory.h"
#include "TFile.h"

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
  std::cout << __METHOD_NAME__ << " " << this->event->histos->Get1DHistogram(0) << std::endl;
  std::cout << __METHOD_NAME__ << histoSum->Get1DHistogram(0) << std::endl;
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
    std::cout << __METHOD_NAME__ << std::endl;
  }

  // loop over histogram specifications and fill
  auto hd = Config::Instance()->GetHistoDefs();  // histogram definitions
  for(auto i : hd)
  {
    this->FillHistogram(i["treeName"].data(), i["histName"], i["nbins"], i["binning"], i["plot"], i["select"]);
  }
}


void EventAnalysis::FillHistogram(std::string treeName, std::string histoName,
                                  std::string nbins,    std::string binning,
                                  std::string plot,     std::string selection)
{
  if(Config::Instance()->Debug())
  {
    std::cout << __METHOD_NAME__ << std::endl;
  }
  double xlow=0.0, xhigh=0.0;
  double ylow=0.0, yhigh=0.0;
  int ndim = Config::Dimension(nbins);
  int nxbin=0, nybin=0;

  if(ndim == 1)
  {
    nxbin = Config::NBins(nbins,1);
    Config::Binning(binning,1,xlow,xhigh);
  }
  else if(ndim == 2)
  {
    nxbin = Config::NBins(nbins,1);
    nybin = Config::NBins(nbins,2);
    Config::Binning(binning,1,xlow,xhigh);
    Config::Binning(binning,2,ylow,yhigh);
  }

  if (!chain)
  {std::cout << __METHOD_NAME__ << "Error no tree found by name: " << treeName << std::endl; exit(1);}

  if(Config::Instance()->Debug())
  {
    std::cout << __METHOD_NAME__ << treeName << " " << histoName << " " << plot << std::endl;
  }

  auto pltSav = histoName;
  auto pltCmd = plot+" >> "+pltSav;

  // create histograms
  if(ndim == 1)
  {
    auto h = new TH1D(pltSav.c_str(),pltSav.c_str(), nxbin, xlow, xhigh);
    h->AddDirectory(kTRUE);
  }
  else if(ndim == 2)
  {
    auto h = new TH2D(pltSav.c_str(),pltSav.c_str(), nxbin, xlow, xhigh, nybin, ylow, yhigh);
    h->AddDirectory(kTRUE);
  }

  auto n = chain->Draw(pltCmd.c_str(),selection.c_str(),"goff");

  std::cout << n << std::endl;
  if(ndim == 1)
  {
    auto h = (TH1*)gDirectory->Get(pltSav.c_str());
    this->histogramNames.push_back(pltSav);
    this->histograms1D[pltSav] = h;
    std::cout << h << std::endl;
  }
  else if(ndim == 2)
  {
    auto h = (TH2*)gDirectory->Get(pltSav.c_str());
    this->histogramNames.push_back(pltSav);
    this->histograms2D[pltSav] = h;
    std::cout << h << std::endl;
  }


  if(Config::Instance()->Debug())
  {
    std::cout << __METHOD_NAME__ << "`" << pltSav << "'  `" << pltCmd << "' " << gDirectory->Get(pltSav.c_str()) << std::endl;
  }
}

void EventAnalysis::Write(TFile *outputFile)
{
  if(Config::Instance()->Debug())
  {
    std::cout << __METHOD_NAME__ << std::endl;
  }

  // write rebdsim histograms
  TDirectory *rebdsimDir = outputFile->mkdir("rebdsimHistorgrams");
  rebdsimDir->cd();
  for(auto h : histograms1D)
  {
    h.second->Write();
  }
  for(auto h : histograms2D)
  {
    h.second->Write();
  }

  // write run merged run histograms
  TDirectory *bdsimDir = outputFile->mkdir("bdsimEventMergedHistograms");
  bdsimDir->cd();

  for(auto h : this->histoSum->Get1DHistograms())
  {
    h->Write();
  }
  for(auto h :this->histoSum->Get2DHistograms())
  {
    h->Write();
  }
}