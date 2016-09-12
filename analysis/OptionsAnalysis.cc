#include "Options.hh"
#include "OptionsAnalysis.hh"

#include "TROOT.h"

class TChain;

ClassImp(OptionsAnalysis)

OptionsAnalysis::OptionsAnalysis():
Analysis("Options.", nullptr, "bdsimOptionsMergedHistograms"),
  options(nullptr)
{;}

OptionsAnalysis::OptionsAnalysis(Options* optionsIn, TChain* chain, bool debug):
  Analysis("Options.", chain, "bdsimOptionsMergedHistograms", debug),
  options(optionsIn)
{;}

OptionsAnalysis::~OptionsAnalysis()
{
  delete options;
}
