#include "OptionsAnalysis.hh"

#include "rebdsim.hh"
#include "Config.hh"
#include "TROOT.h"

ClassImp(OptionsAnalysis)

OptionsAnalysis::OptionsAnalysis():
  Analysis("Options.")
{
  options = nullptr;
}

OptionsAnalysis::~OptionsAnalysis()
{
  delete options;
}

OptionsAnalysis::OptionsAnalysis(Options* optionsIn, TChain* chainIn):
  Analysis("Options.")
{
  chainIn->GetEntry(0);
  chain = chainIn;
  options = optionsIn;
}
