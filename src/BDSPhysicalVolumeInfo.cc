#include "BDSPhysicalVolumeInfo.hh"
#include "globals.hh"
#include "G4String.hh"

BDSPhysicalVolumeInfo::BDSPhysicalVolumeInfo(G4double sPosAtMiddleOfElement) :
    spos(sPosAtMiddleOfElement)
{
  name = "unknown";
  placementName = "unknown";
  precisionRegion = false;
  beamlineIndex = -1;
}

BDSPhysicalVolumeInfo::BDSPhysicalVolumeInfo(G4String nameIn,
                                             G4String placementNameIn,
                                             G4double sPosIn,
                                             G4bool precisionRegionIn,
                                             G4int beamlineIndexIn) :
    name(nameIn), placementName(placementNameIn),
    spos(sPosIn), precisionRegion(precisionRegionIn),
    beamlineIndex(beamlineIndexIn)
{ ; }

BDSPhysicalVolumeInfo::~BDSPhysicalVolumeInfo()
{ ; }

std::ostream &operator<<(std::ostream &out, BDSPhysicalVolumeInfo const &info)
{
  out << "Name: \"" << info.name << "\" S pos: " << info.spos << " mm Precision: "
  << info.precisionRegion;
  return out;
}
