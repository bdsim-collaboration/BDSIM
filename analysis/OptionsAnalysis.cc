#include "Options.hh"
#include "OptionsAnalysis.hh"

#include "TROOT.h"

class TChain;

ClassImp(OptionsAnalysis)

OptionsAnalysis::OptionsAnalysis():
Analysis("Options.", nullptr, "OptionsHistogramsMerged"),
  options(nullptr)
{;}

OptionsAnalysis::OptionsAnalysis(Options* optionsIn, TChain* chain, bool debug):
  Analysis("Options.", chain, "OptionsHistogramsMerged", debug),
  options(optionsIn)
{;}

OptionsAnalysis::~OptionsAnalysis()
{
  delete options;
}
