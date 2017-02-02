#include "BDSEventInfo.hh"
#include "BDSOutputROOTEventInfo.hh"

#include "globals.hh"

BDSEventInfo::BDSEventInfo():
info(new BDSOutputROOTEventInfo())
{;}

BDSEventInfo::~BDSEventInfo()
{
  delete info;
}

void BDSEventInfo::Print() const
{
  G4cout << "Start / stop time (ms): " << info->startTime << " " << info->stopTime << G4endl;
  G4cout << "Duration (ms)         : " << info->duration << G4endl;
}
