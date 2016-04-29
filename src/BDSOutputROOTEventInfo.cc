#include "BDSOutputROOTEventInfo.hh"

#include <ctime>

ClassImp(BDSOutputROOTEventInfo);

BDSOutputROOTEventInfo::BDSOutputROOTEventInfo():
  startTime(0.0), stopTime(0.0), eventDuration(0)
{;}

BDSOutputROOTEventInfo::BDSOutputROOTEventInfo(time_t startTimeIn,
					       time_t stopTimeIn):
  startTime(startTimeIn),
  stopTime(stopTimeIn)
{
  eventDuration = (float)difftime(stopTime,stopTime);
}

BDSOutputROOTEventInfo::~BDSOutputROOTEventInfo()
{;}
