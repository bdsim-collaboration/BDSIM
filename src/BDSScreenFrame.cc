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
	SetDefaultVisAtts();
}

BDSScreenFrame::~BDSScreenFrame()
{
  delete logVol;
	delete visAtt;
}


void BDSScreenFrame::SetDefaultVisAtts(){
	visAtt = new G4VisAttributes(G4Color(0.7,0.7,0.7,0.8));
	visAtt->SetForceSolid(true);
	visAtt->SetVisibility(true);
}

void BDSScreenFrame::SetVisAtts(G4LogicalVolume* logVolIn, G4VisAttributes* visAttIn) {
	if (logVolIn != nullptr) {
		if (visAttIn == nullptr) {
			logVolIn->SetVisAttributes(visAtt);
		} else {
			logVolIn->SetVisAttributes(visAttIn);
		}
	}
}

/// Set the default logical volume to the default visual attributes
void BDSScreenFrame::SetVisAtts(){
	SetVisAtts(logVol);
}