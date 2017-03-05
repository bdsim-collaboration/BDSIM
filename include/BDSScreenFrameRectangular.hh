//
// Created by Lawrence Deacon on 01/03/2017.
//

#ifndef BDSIM_BDSRECTSCREENFRAME_H
#define BDSIM_BDSRECTSCREENFRAME_H

#include "BDSScreenFrame.hh"

class BDSRectScreenFrame : public BDSScreenFrame{
public:
    ~BDSRectScreenFrame();
    BDSRectScreenFrame(G4String name, G4ThreeVector size, G4TwoVector windowSize, G4TwoVector windowOffset,
            G4Material* material);
    G4LogicalVolume* CavityLogVol(){return _cavityLogVol;}
virtual void Build();
    void Place(G4RotationMatrix* rot, G4ThreeVector pos, G4LogicalVolume* motherVol);
private:
    BDSRectScreenFrame();
    G4LogicalVolume* _cavityLogVol;
    G4String _cavityName;
    G4ThreeVector _cavityPos;
    G4RotationMatrix* _zeroRot;
};


#endif //BDSIM_BDSRECTSCREENFRAME_H
