#include "RunAnalysis.hh"

#include "rebdsim.hh"
#include "Config.hh"

ClassImp(RunAnalysis)

RunAnalysis::RunAnalysis():
Analysis("Run.", nullptr, "bdsimRunMergedHistograms"),
  run(nullptr)
{;}

RunAnalysis::RunAnalysis(Run* runIn, TChain* chain, bool debug):
  Analysis("Run.", chain, "bdsimRunMergedHistograms", debug),
  run(runIn)
{
  if (debug)
  {
    std::cout << __METHOD_NAME__ << " " << runIn << " " << chain <<  " "
	      << chain->GetEntries() << std::endl;
  }
}

RunAnalysis::~RunAnalysis()
{;}

void RunAnalysis::Process()
{
  if (debug)
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
