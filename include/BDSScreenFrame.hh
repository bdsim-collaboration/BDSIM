//
// Created by Lawrence Deacon on 01/03/2017.
//

// A frame for the vacuum window in e.g. BDSMultilayerScreen. This is a virtual class. Subtypes are "rect" and
//"rtrack" (rectangular and racetrack frames).

#ifndef BDSIM_BDSSCREENFRAME_H
#define BDSIM_BDSSCREENFRAME_H

#include "G4TwoVector.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"


class BDSScreenFrame {
public:
    virtual ~BDSScreenFrame();
    G4LogicalVolume* LogVol(){return _logVol;}
    virtual void Build() = 0;
protected:
    BDSScreenFrame(G4String name, G4ThreeVector size, G4TwoVector windowSize, G4TwoVector windowOffset,
                   G4Material* material);
    BDSScreenFrame();
    G4LogicalVolume* _logVol;
    G4TwoVector _windowOffset, _windowSize;
    G4ThreeVector _size;
    G4String _name;
    G4Material* _material;
};



#endif //BDSIM_BDSSCREENFRAME_H
