#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSScreenFrameRectangular.hh"

#include "globals.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"


BDSScreenFrameRectangular::BDSScreenFrameRectangular(G4String      name,
						     G4ThreeVector size,
						     G4TwoVector   windowSize,
						     G4TwoVector   windowOffset,
						     G4Material*   material):
  BDSScreenFrame(name, size, windowSize, windowOffset, material),
  zeroRot(new G4RotationMatrix(0,0,0))
{
  Build();
}

BDSScreenFrameRectangular::~BDSScreenFrameRectangular()
{
  delete zeroRot;
}

void BDSScreenFrameRectangular::Build()
{
  G4double lenSaf = BDSGlobalConstants::Instance()->LengthSafety();
  //G4double tinyLenSaf = 1e-3*CLHEP::nm;

  G4Box* frameBox = new G4Box("frameBox",
			      size.x()/2.0,
			      size.y()/2.0,
			      size.z()/2.0);


  G4Box* windowBox = new G4Box("windowBox",
			       windowSize.x()/2.0 + lenSaf,
			       windowSize.y()/2.0 + lenSaf,
			       size.z()/2.0 + lenSaf); // must be thicker than frame.

	cavityPos = G4ThreeVector(windowOffset.x(), windowOffset.y(), 0);
	G4SubtractionSolid* frameSS = new G4SubtractionSolid((G4String)"frameSS", frameBox, windowBox, zeroRot, cavityPos);
	logVol = new G4LogicalVolume(frameSS, material, name+"_lv");
  SetVisAtts();
}

void BDSScreenFrameRectangular::Place(G4RotationMatrix* rot,
				      G4ThreeVector     pos,
				      G4LogicalVolume*  motherVol)
{
  new G4PVPlacement(rot,
		    pos,
		    logVol,
		    "screenFrame_pv",
		    motherVol,
		    false,
		    0,
		    checkOverlaps);

}

void BDSScreenFrameRectangular::SetVisAtts()
{
  BDSScreenFrame::SetVisAtts(logVol);
}
