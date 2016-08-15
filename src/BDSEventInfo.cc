#include "BDSEventInfo.hh"

#include "globals.hh"

BDSEventInfo::BDSEventInfo()
{;}

BDSEventInfo::~BDSEventInfo()
{;}

void BDSEventInfo::Print() const
{
  G4cout << "Start / stop time (ms): " << startTime << " " << stopTime << G4endl;
  G4cout << "Duration (ms)         : " << duration << G4endl;
}
