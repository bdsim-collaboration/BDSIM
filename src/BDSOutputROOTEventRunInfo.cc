#include "BDSOutputROOTEventInfo.hh"
#include "BDSOutputROOTEventRunInfo.hh"

#include <ctime>

ClassImp(BDSOutputROOTEventRunInfo)

BDSOutputROOTEventRunInfo::BDSOutputROOTEventRunInfo():
  startTime(time_t()), stopTime(time_t()), duration(0), seedStateAtStart("")
{;}

BDSOutputROOTEventRunInfo::BDSOutputROOTEventRunInfo(const BDSOutputROOTEventInfo* info):
  startTime(info->startTime),
  stopTime(info->stopTime),
  duration(info->duration),
  seedStateAtStart(info->seedStateAtStart)
{;}

BDSOutputROOTEventRunInfo::~BDSOutputROOTEventRunInfo()
{;}

void BDSOutputROOTEventRunInfo::Flush()
{
  startTime        = time_t();
  stopTime         = time_t();
  duration         = 0;
  seedStateAtStart = "";
}
