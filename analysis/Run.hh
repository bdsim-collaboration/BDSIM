#ifndef __RUN_H
#define __RUN_H

#include "TChain.h"

#include "BDSOutputROOTEventRunInfo.hh"
#include "BDSOutputROOTEventHistograms.hh"

class Run
{
public :
  Run();
  virtual ~Run();

  void SetBranchAddress(TChain *);

  BDSOutputROOTEventRunInfo                      *info;
  BDSOutputROOTEventHistograms                   *histos;

private:

  ClassDef(Run,1);
};


#endif