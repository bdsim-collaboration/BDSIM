#ifndef OPTIONSANALYSIS_H
#define OPTIONSANALYSIS_H

#include "TROOT.h"

#include "Analysis.hh"

class Options;
class TChain;

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
