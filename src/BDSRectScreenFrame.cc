#include "BDSGlobalConstants.hh"
#include "BDSRectScreenFrame.hh"

#include "globals.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"


BDSRectScreenFrame::BDSRectScreenFrame(G4String      name,
				       G4ThreeVector size,
				       G4TwoVector   windowSize,
                                       G4TwoVector   windowOffset,
				       G4Material*   material):
  BDSScreenFrame(name, size, windowSize, windowOffset, material)
{
  Build();
}

BDSRectScreenFrame::~BDSRectScreenFrame()
{;}

void BDSRectScreenFrame::Build()
{
  G4double lenSaf = BDSGlobalConstants::Instance()->LengthSafety();

  G4Box* frameBox = new G4Box("frameBox",
			      size.x()/2.0,
			      size.y()/2.0,
			      size.z()/2.0);
  
  G4Box* windowBox = new G4Box("windowBox",
			       windowSize.x()/2.0+lenSaf,
			       windowSize.y()/2.0+lenSaf,
			       size.z()); // z long for unambiguous subtraction
  
  G4RotationMatrix* zeroRot = new G4RotationMatrix(0,0,0);
  G4ThreeVector pos(windowOffset.x(), windowOffset.y(), 0);
  
  G4SubtractionSolid* frameSolid = new G4SubtractionSolid("frame_solid",
							  frameBox,
							  windowBox,
							  zeroRot,
							  pos);

  logVol = new G4LogicalVolume(frameSolid, material, name+"_lv");
}
