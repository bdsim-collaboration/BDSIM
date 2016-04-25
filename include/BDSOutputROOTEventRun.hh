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
  double startTime;
  double endTime;
  double runTime;

  BDSOutputROOTEventHistograms *histo = nullptr;

  ClassDef(BDSOutputROOTEventRun,1);
};

#endif
