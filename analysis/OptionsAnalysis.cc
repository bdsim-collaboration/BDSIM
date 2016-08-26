#include "OptionsAnalysis.hh"

#include "TROOT.h"
#include "TChain.h"

#include "Options.hh"

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
