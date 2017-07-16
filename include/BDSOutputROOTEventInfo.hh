#ifndef BDSOUTPUTROOTEVENTINFO_H
#define BDSOUTPUTROOTEVENTINFO_H

#include "TROOT.h"
#include "TObject.h"

#include <ctime>

/**
 * @brief Information pertaining to an individual event.
 *
 * @author Stewart Boogert
 */

class BDSOutputROOTEventInfo: public TObject
{  
public:
  BDSOutputROOTEventInfo();

  virtual ~BDSOutputROOTEventInfo();
  void Flush();
  
  time_t startTime; ///< Time stamp at start of event.
  time_t stopTime;  ///< Time stamp at end of event.
  float  duration;  ///< Number of seconds event took to complete simulation (not writing out).
  std::string seedStateAtStart; ///< Seed state at the start of the event.
  int    index;     ///< Number of this event or run.

  ClassDef(BDSOutputROOTEventInfo,1);
};

#endif
