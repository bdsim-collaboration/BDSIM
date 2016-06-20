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

//  std::cout << __METHOD_NAME__ << " " << this->event->histos->Get1DHistogram(0) << std::endl;
//  std::cout << __METHOD_NAME__ << histoSum->Get1DHistogram(0) << std::endl;
}

EventAnalysis::~EventAnalysis()
{;}

void EventAnalysis::Process()
{
  if(Config::Instance()->Debug())
  {
    std::cout << __METHOD_NAME__ << this->chain->GetEntries() << " " << std::endl;
  }
  // loop over events
  for(int i=0;i<this->chain->GetEntries();++i) {
    this->chain->GetEntry(i);

    if(i==0)
    {
      histoSum = new HistogramMerge(event->histos);
    }

    histoSum->Add(event->histos);


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
//      std::cout << "EventAnalysis::Process> " << this->event->sampler->samplerName << std::endl;
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
      std::cout << "EventAnalysis::ProcessSamplers> " << (*s)->s->samplerName << " " << (*s)->s->n <<std::endl;
    }

    // process samplers
    (*s)->Process();
  }

}

void EventAnalysis::Initialise()
{
  for(auto i = this->samplerAnalyses.begin(); i != this->samplerAnalyses.end(); ++i)
    {(*i)->Initialise();}
}

void EventAnalysis::Terminate()
{
  for(auto i = this->samplerAnalyses.begin(); i != this->samplerAnalyses.end(); ++i)
  {
    (*i)->Terminate();
    this->opticalFunctions.push_back((*i)->GetOpticalFunctions());
  }

  histoSum->Terminate();
}

void EventAnalysis::SimpleHistograms()
{
  if(Config::Instance()->Debug())
    {std::cout << __METHOD_NAME__ << std::endl;}

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
    {std::cout << __METHOD_NAME__ << std::endl;}
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

  chain->Draw(pltCmd.c_str(),selection.c_str(),"goff");

  if(ndim == 1)
  {
    auto h = (TH1*)gDirectory->Get(pltSav.c_str());
    this->histogramNames.push_back(pltSav);
    this->histograms1D[pltSav] = h;
    // std::cout << h << std::endl;
  }
  else if(ndim == 2)
  {
    auto h = (TH2*)gDirectory->Get(pltSav.c_str());
    this->histogramNames.push_back(pltSav);
    this->histograms2D[pltSav] = h;
    // std::cout << h << std::endl;
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
  this->histoSum->Write(outputFile);


  outputFile->cd("/");

  std::vector<double> xOpticsPoint;
  std::vector<double> yOpticsPoint;
  xOpticsPoint.resize(24);
  yOpticsPoint.resize(24);

  // write optical functions
  TTree *opticsTree = new TTree("optics","optics");
  opticsTree->Branch("Emitt_x", &(xOpticsPoint[0]), "Emitt_x/D");
  opticsTree->Branch("Emitt_y", &(yOpticsPoint[0]), "Emitt_y/D");
  opticsTree->Branch("Alpha_x", &(xOpticsPoint[1]), "Alpha_x/D");
  opticsTree->Branch("Alpha_y", &(yOpticsPoint[1]), "Alpha_y/D");
  opticsTree->Branch("Beta_x",  &(xOpticsPoint[2]), "Beta_x/D");
  opticsTree->Branch("Beta_y",  &(yOpticsPoint[2]), "Beta_y/D");
  opticsTree->Branch("Gamma_x", &(xOpticsPoint[3]), "Gamma_x/D");
  opticsTree->Branch("Gamma_y", &(yOpticsPoint[3]), "Gamma_y/D");
  opticsTree->Branch("Disp_x",  &(xOpticsPoint[4]), "Disp_x/D");
  opticsTree->Branch("Disp_y",  &(yOpticsPoint[4]), "Disp_y/D");
  opticsTree->Branch("Disp_xp", &(xOpticsPoint[5]), "Disp_xp/D");
  opticsTree->Branch("Disp_yp", &(yOpticsPoint[5]), "Disp_yp/D");
  opticsTree->Branch("Mean_x",  &(xOpticsPoint[6]), "Mean_x/D");
  opticsTree->Branch("Mean_y",  &(yOpticsPoint[6]), "Mean_y/D");
  opticsTree->Branch("Mean_xp", &(xOpticsPoint[7]), "Mean_xp/D");
  opticsTree->Branch("Mean_yp", &(yOpticsPoint[7]), "Mean_yp/D");
  opticsTree->Branch("Sigma_x", &(xOpticsPoint[8]), "Sigma_x/D");
  opticsTree->Branch("Sigma_y", &(yOpticsPoint[8]), "Sigma_y/D");
  opticsTree->Branch("Sigma_xp",&(xOpticsPoint[9]), "Sigma_xp/D");
  opticsTree->Branch("Sigma_yp",&(yOpticsPoint[9]), "Sigma_yp/D");
  opticsTree->Branch("S"       ,&(xOpticsPoint[10]),"S/D");
  opticsTree->Branch("Npart"   ,&(xOpticsPoint[11]),"Npart/D");
  
  opticsTree->Branch("Sig_Emitt_x", &(xOpticsPoint[12]), "Sig_Emitt_x/D");
  opticsTree->Branch("Sig_Emitt_y", &(yOpticsPoint[12]), "Sig_Emitt_y/D");
  opticsTree->Branch("Sig_Alpha_x", &(xOpticsPoint[13]), "Sig_Alpha_x/D");
  opticsTree->Branch("Sig_Alpha_y", &(yOpticsPoint[13]), "Sig_Alpha_y/D");
  opticsTree->Branch("Sig_Beta_x",  &(xOpticsPoint[14]), "Sig_Beta_x/D");
  opticsTree->Branch("Sig_Beta_y",  &(yOpticsPoint[14]), "Sig_Beta_y/D");
  opticsTree->Branch("Sig_Gamma_x", &(xOpticsPoint[15]), "Sig_Gamma_x/D");
  opticsTree->Branch("Sig_Gamma_y", &(yOpticsPoint[15]), "Sig_Gamma_y/D");
  opticsTree->Branch("Sig_Disp_x",  &(xOpticsPoint[16]), "Sig_Disp_x/D");
  opticsTree->Branch("Sig_Disp_y",  &(yOpticsPoint[16]), "Sig_Disp_y/D");
  opticsTree->Branch("Sig_Disp_xp", &(xOpticsPoint[17]), "Sig_Disp_xp/D");
  opticsTree->Branch("Sig_Disp_yp", &(yOpticsPoint[17]), "Sig_Disp_yp/D");
  opticsTree->Branch("Sig_Mean_x",  &(xOpticsPoint[18]), "Sig_Mean_x/D");
  opticsTree->Branch("Sig_Mean_y",  &(yOpticsPoint[18]), "Sig_Mean_y/D");
  opticsTree->Branch("Sig_Mean_xp", &(xOpticsPoint[19]), "Sig_Mean_xp/D");
  opticsTree->Branch("Sig_Mean_yp", &(yOpticsPoint[19]), "Sig_Mean_yp/D");
  opticsTree->Branch("Sig_Sigma_x", &(xOpticsPoint[20]), "Sig_Sigma_x/D");
  opticsTree->Branch("Sig_Sigma_y", &(yOpticsPoint[20]), "Sig_Sigma_y/D");
  opticsTree->Branch("Sig_Sigma_xp",&(xOpticsPoint[21]), "Sig_Sigma_xp/D");
  opticsTree->Branch("Sig_Sigma_yp",&(yOpticsPoint[21]), "Sig_Sigma_yp/D");

  
  for(auto i : this->opticalFunctions)
  {
    xOpticsPoint = i[0];
    yOpticsPoint = i[1];
    opticsTree->Fill();
  }
  opticsTree->Write();

}
