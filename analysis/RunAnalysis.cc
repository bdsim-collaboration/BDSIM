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

  //chainIn->GetEntry(0);

  this->run   = runIn;
  this->chain = chainIn;
}


RunAnalysis::~RunAnalysis()
{

}

void RunAnalysis::Process()
{
  if(Config::Instance()->Debug())
  {
    std::cout << __METHOD_NAME__ << this->chain->GetEntries() << " " << std::endl;
  }
  // loop over events
  for(int i=0;i<this->chain->GetEntries();++i)
  {
    this->chain->GetEntry(i);

    if (i == 0)
    {
      histoSum = new BDSOutputROOTEventHistograms(*(run->histos));
    }
  }

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
