#include "BDSOutputROOTEventRunInfo.hh"

#include <ctime>

ClassImp(BDSOutputROOTEventRunInfo)

BDSOutputROOTEventRunInfo::BDSOutputROOTEventRunInfo():
  startTime(time_t()), stopTime(time_t()), duration(0), seedStateAtStart("")
{;}

BDSOutputROOTEventRunInfo::~BDSOutputROOTEventRunInfo()
{;}
