#include "Options.hh"
#include "OptionsAnalysis.hh"

#include "TROOT.h"

class TChain;

ClassImp(OptionsAnalysis)

OptionsAnalysis::OptionsAnalysis():
  Analysis("Options.", nullptr, "OptionsHistogramsMerged"),
  options(nullptr)
{;}

OptionsAnalysis::OptionsAnalysis(Options* optionsIn, TChain* chainIn, bool debugIn):
  Analysis("Options.", chainIn, "OptionsHistogramsMerged", debugIn),
  options(optionsIn)
{;}

OptionsAnalysis::~OptionsAnalysis()
{
  delete options;
}
