#ifndef BDSOUTPUTROOTEVENTRUN_H
#define BDSOUTPUTROOTEVENTRUN_H

#include "TROOT.h"

class BDSOutputROOTEventRunInfo
{

public:
  BDSOutputROOTEventRunInfo();
  virtual ~BDSOutputROOTEventRunInfo();
private:
  double startTime;
  double endTime;
  double runTime;


  ClassDef(BDSOutputROOTEventRunInfo,1);
};

#endif
