#ifndef OPTIONSANALYSIS_H
#define OPTIONSANALYSIS_H

#include "TROOT.h"
#include "TFile.h"
#include "TChain.h"

#include "OptionsAnalysis.hh"
#include "Analysis.hh"
#include "Options.hh"


class OptionsAnalysis : public Analysis
{
public:
  OptionsAnalysis();
  OptionsAnalysis(Options* optionsIn, TChain* chainIn);
  virtual ~OptionsAnalysis();

  virtual void Process(){};
  virtual void Terminate(){};

protected:
  Options* options;
  ClassDef(OptionsAnalysis,1);
};

#endif
