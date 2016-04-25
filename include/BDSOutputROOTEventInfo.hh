#ifndef BDSOUTPUTROOTEVENTINFO_H
#define BDSOUTPUTROOTEVENTINFO_H

#include "TROOT.h"

class BDSOutputROOTEventInfo
{
public :
  double startTime;
  double endTime;
  double eventTime;

  BDSOutputROOTEventInfo();
  virtual ~BDSOutputROOTEventInfo();

  ClassDef(BDSOutputROOTEventInfo,1);
};

#endif