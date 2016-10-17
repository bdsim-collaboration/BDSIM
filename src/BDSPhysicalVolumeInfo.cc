#include "BDSPhysicalVolumeInfo.hh"
#include "G4Types.hh"
#include "G4String.hh"

#include <ostream>

BDSPhysicalVolumeInfo::BDSPhysicalVolumeInfo(G4double sPosAtMiddleOfElement):
  spos(sPosAtMiddleOfElement)
{
  name          = "unknown";
  placementName = "unknown";
  beamlineIndex = -1;
}

BDSPhysicalVolumeInfo::BDSPhysicalVolumeInfo(G4String nameIn,
                                             G4String placementNameIn,
                                             G4double sPosIn,
                                             G4int    beamlineIndexIn):
    name(nameIn),
    placementName(placementNameIn),
    spos(sPosIn),
    beamlineIndex(beamlineIndexIn)
{;}

BDSPhysicalVolumeInfo::~BDSPhysicalVolumeInfo()
{;}

std::ostream &operator<<(std::ostream &out, BDSPhysicalVolumeInfo const &info)
{
  out << "Name: \"" << info.name << "\" S pos: " << info.spos << " mm Precision: ";
  return out;
}
