#ifndef __RUNANALYSIS_H
#define __RUNANALYSIS_H

#include "TROOT.h"
#include "TFile.h"
#include "TChain.h"

#include "BDSOutputROOTEventHistograms.hh"

#include "Run.hh"
#include "Analysis.hh"

class RunAnalysis : public Analysis
{
public:
  RunAnalysis();
  RunAnalysis(Run *r, TChain *c);
  virtual ~RunAnalysis();

  virtual void Process();
  virtual void Write(TFile *f);

protected:
  Run    *run;

  ClassDef(RunAnalysis,1);
};

#endif
