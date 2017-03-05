//
// Created by Lawrence Deacon on 01/03/2017.
//

#include "BDSRectScreenFrame.hh"
#include "BDSGlobalConstants.hh"
#include "G4SubtractionSolid.hh"
#include "G4RotationMatrix.hh"
#include "G4Box.hh"
#include "BDSMaterials.hh"
#include "G4PVPlacement.hh"

BDSRectScreenFrame::BDSRectScreenFrame(){;}
BDSRectScreenFrame::~BDSRectScreenFrame(){
    delete _cavityLogVol;
    delete _zeroRot;
}

BDSRectScreenFrame::BDSRectScreenFrame(G4String name, G4ThreeVector size, G4TwoVector windowSize,
                                       G4TwoVector windowOffset, G4Material* material):
        BDSScreenFrame(name, size, windowSize, windowOffset, material) {
    Build();
}

void BDSRectScreenFrame::Build(){
    G4double lenSaf = BDSGlobalConstants::Instance()->LengthSafety();
    G4Box* frameBox = new G4Box("frameBox",_size.x()/2.0, _size.y()/2.0, _size.z()/2.0);
    G4Box* windowBox = new G4Box("windowBox",_windowSize.x()/2.0+lenSaf,_windowSize.y()/2.0+lenSaf,_size.z()/2.0);
    //G4SubtractionSolid* frameSolid = new G4SubtractionSolid("frameSolid",frameBox, windowBox, zeroRot, pos);
   // _logVol = new G4LogicalVolume(frameSolid, _material, _name);
    _logVol = new G4LogicalVolume(frameBox, _material, _name);
    _cavityName = _name + "_cavity";
    _cavityLogVol = new G4LogicalVolume(windowBox, BDSMaterials::Instance()->GetMaterial("vacuum"), _cavityName);
    _cavityPos = G4ThreeVector(_windowOffset.x(), _windowOffset.y(), 0);
}

void BDSRectScreenFrame::Place(G4RotationMatrix* rot, G4ThreeVector pos, G4LogicalVolume* motherVol){
    new G4PVPlacement(rot,
                      pos,
                      _logVol,
                      "screenFrame",
                      motherVol,
                      false,
                      0,
                      true
    );
    //Place a cavity inside the frame box.
    _zeroRot = new G4RotationMatrix(0,0,0);
    new G4PVPlacement(_zeroRot,
                      _cavityPos,
                      _cavityLogVol,
                      "screenFrameCavity",
                      _logVol,
                      false,
                      0,
                      true
    );
}