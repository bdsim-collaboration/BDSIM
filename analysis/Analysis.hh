#ifndef ANALYSIS_ANALYSIS_H
#define ANALYSIS_ANALYSIS_H

#include "TChain.h"

class Analysis
{
public:

  Analysis();
  virtual ~Analysis();

  //Method which calls all other methods.
  virtual void Execute();
  virtual void Process() = 0;
  virtual void SimpleHistograms() = 0;
  virtual void Terminate() = 0;
  virtual void Write(TFile *f) = 0;

protected:
  TChain *chain;
};

#endif
