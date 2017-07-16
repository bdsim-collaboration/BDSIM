#ifndef BDSOUTPUTROOTEVENTRUN_H
#define BDSOUTPUTROOTEVENTRUN_H

#include "TROOT.h"
#include "TObject.h"

#include <ctime>

class BDSOutputROOTEventInfo;

/**
 * @brief Information pertaining to a run.
 *
 * @author Stewart Boogert
 */

class BDSOutputROOTEventRunInfo: public TObject
{
public:
  BDSOutputROOTEventRunInfo();
  BDSOutputROOTEventRunInfo(const BDSOutputROOTEventInfo* info);
  virtual ~BDSOutputROOTEventRunInfo();
  void Flush();

  time_t startTime;
  time_t stopTime;
  double duration;
  std::string seedStateAtStart; ///< Seed state at the start of the event.
  
  ClassDef(BDSOutputROOTEventRunInfo,1);
};

#endif
