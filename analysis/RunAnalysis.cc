#include "RunAnalysis.hh"

#include "rebdsim.hh"
#include "Config.hh"

ClassImp(RunAnalysis)

RunAnalysis::RunAnalysis():
  Analysis("Run.")
{;}

RunAnalysis::RunAnalysis(Run *runIn, TChain *chainIn):
  Analysis("Run.")
{
  if(Config::Instance()->Debug())
  {
    std::cout << __METHOD_NAME__ << " " << runIn << " " << chainIn <<  " " << chainIn->GetEntries() << std::endl;
  }

  //chainIn->GetEntry(0);
  run   = runIn;
  chain = chainIn;
}

RunAnalysis::~RunAnalysis()
{;}

void RunAnalysis::Process()
{
  if(Config::Instance()->Debug())
  {std::cout << __METHOD_NAME__ << this->chain->GetEntries() << " " << std::endl;}
  // loop over events
  for(int i=0; i < chain->GetEntries(); ++i)
  {
    chain->GetEntry(i);

    if (i == 0)
      {histoSum = new HistogramMerge(run->histos);}
    else
      {histoSum->Add(run->histos);}
  }
}

void RunAnalysis::Write(TFile *outputFile)
{
  Analysis::Write(outputFile);
  TDirectory *bdsimDir = outputFile->mkdir("bdsimRunMergedHistograms");
  bdsimDir->cd();
  histoSum->Write(outputFile);
}
