#ifndef BDSOUTPUTROOTEVENTRUN_H
#define BDSOUTPUTROOTEVENTRUN_H

#include "TROOT.h"

#include <ctime>

class BDSOutputROOTEventRunInfo
{
public:
  BDSOutputROOTEventRunInfo();
  virtual ~BDSOutputROOTEventRunInfo();

  time_t startTime;
  time_t stopTime;
  double duration;

  ClassDef(BDSOutputROOTEventRunInfo,1);
};

#endif
