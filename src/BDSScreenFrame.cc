#include "BDSGlobalConstants.hh"
#include "BDSScreenFrame.hh"

#include "G4LogicalVolume.hh"

BDSScreenFrame::BDSScreenFrame(G4String      name,
			       G4ThreeVector size,
			       G4TwoVector   windowSize,
			       G4TwoVector   windowOffset,
			       G4Material*   material):
  name(name),
  size(size),
  windowSize(windowSize),
  windowOffset(windowOffset),
  material(material),
  logVol(nullptr)
{
  checkOverlaps = BDSGlobalConstants::Instance()->CheckOverlaps();
}

BDSScreenFrame::~BDSScreenFrame()
{
  delete logVol;
}
