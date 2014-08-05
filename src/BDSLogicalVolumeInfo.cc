#include "BDSLogicalVolumeInfo.hh"
#include "globals.hh"
#include "G4String.hh"

BDSLogicalVolumeInfo::BDSLogicalVolumeInfo(){;}

BDSLogicalVolumeInfo::BDSLogicalVolumeInfo(G4double SPosIn)
{
  SetSPos(SPosIn);
}

BDSLogicalVolumeInfo::BDSLogicalVolumeInfo(G4String NameIn, G4double SPosIn)
{
  SetName(NameIn);
  SetSPos(SPosIn);
}

BDSLogicalVolumeInfo::~BDSLogicalVolumeInfo(){;}
