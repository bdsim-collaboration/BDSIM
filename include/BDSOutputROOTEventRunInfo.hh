#ifndef BDSOUTPUTROOTEVENTRUN_H
#define BDSOUTPUTROOTEVENTRUN_H

#include "TROOT.h"

#include <ctime>

/**
 * @brief Information pertaining to a run.
 *
 * @author Stewart Boogert
 */

class BDSOutputROOTEventRunInfo
{
public:
  BDSOutputROOTEventRunInfo();
  virtual ~BDSOutputROOTEventRunInfo();

  time_t startTime;
  time_t stopTime;
  double duration;
  std::string seedStateAtStart; ///< Seed state at the start of the event.
  
  ClassDef(BDSOutputROOTEventRunInfo,1);
};

#endif
