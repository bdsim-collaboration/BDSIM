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
virtual void Build();
private:
    BDSRectScreenFrame();
};


#endif //BDSIM_BDSRECTSCREENFRAME_H
