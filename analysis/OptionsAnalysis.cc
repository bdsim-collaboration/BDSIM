#include "OptionsAnalysis.hh"

#include "TROOT.h"
#include "TChain.h"

#include "Options.hh"

ClassImp(OptionsAnalysis)

OptionsAnalysis::OptionsAnalysis():
Analysis("Options.", nullptr),
  options(nullptr)
{;}

OptionsAnalysis::OptionsAnalysis(Options* optionsIn, TChain* chain, bool debug):
  Analysis("Options.", chain, debug),
  options(optionsIn)
{;}

OptionsAnalysis::~OptionsAnalysis()
{
  delete options;
}
