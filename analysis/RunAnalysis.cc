#include "RunAnalysis.hh"

#include "rebdsim.hh"
#include "Config.hh"

ClassImp(RunAnalysis);

RunAnalysis::RunAnalysis()
{
}

RunAnalysis::RunAnalysis(Run *runIn, TChain *chainIn)
{
  if(Config::Instance()->Debug())
  {
    std::cout << __METHOD_NAME__ << " " << runIn << " " << chainIn <<  " " << chainIn->GetEntries() << std::endl;
  }

  chainIn->GetEntry(0);

  this->run   = runIn;
  this->chain = chainIn;

  std::cout << run << std::endl;
  histoSum = new BDSOutputROOTEventHistograms(*(run->histos));
}


RunAnalysis::~RunAnalysis()
{

}

void RunAnalysis::Write(TFile *outputFile)
{
  if(Config::Instance()->Debug())
  {
    std::cout << __METHOD_NAME__ << std::endl;
  }
  TDirectory *bdsimDir = outputFile->mkdir("bdsimRunMergedHistograms");
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
