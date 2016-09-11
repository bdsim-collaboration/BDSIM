#include "Analysis.hh"
#include "Config.hh"
#include "rebdsim.hh"

#include "TChain.h"

Analysis::Analysis(std::string treeNameIn,
		   TChain*     chainIn,
		   std::string mergedHistogramNameIn,
		   bool        debugIn):
  treeName(treeNameIn),
  chain(chainIn),
  mergedHistogramName(mergedHistogramNameIn),
  histoSum(nullptr),
  debug(debugIn)
{;}

Analysis::~Analysis()
{
  delete chain;
  delete histoSum;
}

void Analysis::Execute()
{
  Process();
  SimpleHistograms();
  Terminate();
}

void Analysis::SimpleHistograms()
{
  if(debug)
    {std::cout << __METHOD_NAME__ << std::endl;}

  // loop over histogram specifications and fill
  auto hd = Config::Instance()->GetHistoDefs();  // histogram definitions
  for(auto i : hd)
  {
    if (i["treeName"] == treeName)
      {
	FillHistogram(i["treeName"].data(), i["histName"], i["nbins"], i["binning"], i["plot"], i["select"]);
      }
  }
}

void Analysis::Terminate()
{
  if (histoSum)
    {histoSum->Terminate();}
}

void Analysis::FillHistogram(std::string treeName, std::string histoName,
			     std::string nbins,    std::string binning,
			     std::string plot,     std::string selection)
{
  if(debug)
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

  if(debug)
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


  if(debug)
  {
    std::cout << __METHOD_NAME__ << "`" << pltSav << "'  `" << pltCmd << "' " << gDirectory->Get(pltSav.c_str()) << std::endl;
  }
}

void Analysis::Write(TFile* outputFile)
{
  //treeName typically has a "." at the end, deleting it here:
  std::string cleanedName = treeName.erase(treeName.size() - 1);
  std::string outputDirName = std::string("rebdsim_") + cleanedName + std::string("_Histograms");
  TDirectory *rebdsimDir = outputFile->mkdir(outputDirName.c_str());
  rebdsimDir->cd();
  for(auto h : histograms1D)
    {h.second->Write();}
  for(auto h : histograms2D)
    {h.second->Write();}
  outputFile->cd("/");

  // Merged Histograms for this analysis instance (could be run, event etc)
  if (histoSum)
    {
      TDirectory* bdsimDir = outputFile->mkdir(mergedHistogramName.c_str());
      bdsimDir->cd();
      histoSum->Write(outputFile);
    }
}
