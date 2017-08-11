#include "BDSColours.hh"
#include "BDSGlobalConstants.hh"
#include "BDSScreenFrame.hh"

#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"

BDSScreenFrame::BDSScreenFrame(G4String      nameIn,
			       G4ThreeVector sizeIn,
			       G4TwoVector   windowSizeIn,
			       G4TwoVector   windowOffsetIn,
			       G4Material*   materialIn):
  name(nameIn),
  size(sizeIn),
  windowSize(windowSizeIn),
  windowOffset(windowOffsetIn),
  material(materialIn),
  logVol(nullptr)
{
  checkOverlaps = BDSGlobalConstants::Instance()->CheckOverlaps();
  SetDefaultVisAtts();
}

BDSScreenFrame::~BDSScreenFrame()
{
  delete logVol;
  delete visAtt;
}

void BDSScreenFrame::SetDefaultVisAtts()
{
  visAtt = new G4VisAttributes(BDSColours::Instance()->GetColour("screenframe"));
	visAtt->SetForceWireframe(true);
  visAtt->SetVisibility(true);
}

void BDSScreenFrame::SetVisAtts(G4LogicalVolume* logVolIn,
				G4VisAttributes* visAttIn)
{
  if (logVolIn)
    {
      if (!visAttIn)
	{logVolIn->SetVisAttributes(visAtt);}
      else
	{logVolIn->SetVisAttributes(visAttIn);}
    }
}

void BDSScreenFrame::SetVisAtts()
{
  SetVisAtts(logVol);
}
