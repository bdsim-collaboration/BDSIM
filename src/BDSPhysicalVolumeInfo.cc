#include "BDSPhysicalVolumeInfo.hh"
#include "globals.hh"
#include "G4String.hh"

BDSPhysicalVolumeInfo::BDSPhysicalVolumeInfo(G4double sPosAtMiddleOfElement):
  spos(sPosAtMiddleOfElement)
{;}

BDSPhysicalVolumeInfo::BDSPhysicalVolumeInfo(G4String nameIn,
					     G4String placemenetNameIn,
					     G4double sPosIn):
  name(nameIn),placemenetName(placementNameIn),spos(sPosIn)
{;}

BDSPhysicalVolumeInfo::~BDSPhysicalVolumeInfo(){;}
