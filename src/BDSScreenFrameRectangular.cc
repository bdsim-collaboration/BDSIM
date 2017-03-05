#include "BDSScreenFrameRectangular.hh"
#include "BDSGlobalConstants.hh"
#include "G4SubtractionSolid.hh"
#include "G4RotationMatrix.hh"
#include "G4Box.hh"
#include "BDSMaterials.hh"
#include "G4PVPlacement.hh"

BDSScreenFrameRectangular::~BDSScreenFrameRectangular(){
    delete cavityLogVol;
    delete zeroRot;
}

BDSScreenFrameRectangular::BDSScreenFrameRectangular(G4String name, G4ThreeVector size, G4TwoVector windowSize,
                                       G4TwoVector windowOffset, G4Material* material):
        BDSScreenFrame(name, size, windowSize, windowOffset, material) {
    Build();
}

void BDSScreenFrameRectangular::Build(){
    G4double lenSaf = BDSGlobalConstants::Instance()->LengthSafety();
    G4Box* frameBox = new G4Box("frameBox",
                                size.x()/2.0,
                                size.y()/2.0,
                                size.z()/2.0);

    G4Box* windowBox = new G4Box("windowBox",
                                 windowSize.x()/2.0+lenSaf,
                                 windowSize.y()/2.0+lenSaf,
                                 size.z()/2.0);

    logVol = new G4LogicalVolume(frameBox, material, name);
    cavityName = name + "_cavity";
    cavityLogVol = new G4LogicalVolume(windowBox,
                                        BDSMaterials::Instance()->GetMaterial("vacuum"),
                                        cavityName);
    cavityPos = G4ThreeVector(windowOffset.x(), windowOffset.y(), 0);
    SetVisAtts();
}

void BDSScreenFrameRectangular::Place(G4RotationMatrix* rot, G4ThreeVector pos, G4LogicalVolume* motherVol){
    new G4PVPlacement(rot,
                      pos,
                      logVol,
                      "screenFrame",
                      motherVol,
                      false,
                      0,
                      true
    );
    //Place a cavity inside the frame box.
    zeroRot = new G4RotationMatrix(0,0,0);
    new G4PVPlacement(zeroRot,
                      cavityPos,
                      cavityLogVol,
                      "screenFrameCavity",
                      logVol,
                      false,
                      0,
                      true
    );
}

void BDSScreenFrameRectangular::SetVisAtts() {
    BDSScreenFrame::SetVisAtts();
    BDSScreenFrame::SetVisAtts(cavityLogVol);
}