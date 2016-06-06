#ifndef BDSOUTPUTROOTEVENTRUN_H
#define BDSOUTPUTROOTEVENTRUN_H

#include "TROOT.h"

#include "BDSOutputROOTEventHistograms.hh"

class BDSOutputROOTEventRun
{

public:
  BDSOutputROOTEventRun();
  virtual ~BDSOutputROOTEventRun();

private:
  double runStartTime;
  double runEndTime;
  double runDuration;

  BDSOutputROOTEventHistograms *histo = nullptr;

  ClassDef(BDSOutputROOTEventRun,1);
};

#endif
