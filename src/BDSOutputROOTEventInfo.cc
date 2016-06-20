#include "BDSOutputROOTEventInfo.hh"

#include <ctime>

ClassImp(BDSOutputROOTEventInfo)

BDSOutputROOTEventInfo::BDSOutputROOTEventInfo():
  startTime(time_t()), stopTime(time_t()), duration(0), seedStateAtStart("")
{;}

BDSOutputROOTEventInfo::~BDSOutputROOTEventInfo()
{;}
