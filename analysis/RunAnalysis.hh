#ifndef __RUNANALYSIS_H
#define __RUNANALYSIS_H

#include "TROOT.h"
#include "TFile.h"
#include "TChain.h"

#include "BDSOutputROOTEventHistograms.hh"

#include "Run.hh"

class RunAnalysis
{
public:
  RunAnalysis();
  RunAnalysis(Run *r, TChain *c);

  virtual ~RunAnalysis();

  void Process();
  void Write(TFile *f);

  BDSOutputROOTEventHistograms  *histoSum;          // bdsim histograms


protected:
  Run    *run;
  TChain *chain;


  ClassDef(RunAnalysis,1);
};

#endif