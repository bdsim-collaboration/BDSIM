#ifndef BDSOUTPUTROOTEVENTINFO_H
#define BDSOUTPUTROOTEVENTINFO_H

#include "TROOT.h"

#include <ctime>

class BDSOutputROOTEventInfo
{  
public:
  BDSOutputROOTEventInfo();
  BDSOutputROOTEventInfo(time_t startTimeIn,
			 time_t stopTimeIn);

  virtual ~BDSOutputROOTEventInfo();

  ClassDef(BDSOutputROOTEventInfo,1);
  
  time_t startTime; ///< Time stamp at start of event.
  time_t stopTime;  ///< Time stamp at end of event.
  float  eventDuration; ///< Number of seconds event took to complete simulation (not writing out).
};

#endif
