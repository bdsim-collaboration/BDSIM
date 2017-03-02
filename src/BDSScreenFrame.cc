//
// Created by Lawrence Deacon on 01/03/2017.
//

#include <include/BDSGlobalConstants.hh>
#include "BDSScreenFrame.hh"
#include "G4Box.hh"
#include "BDSGlobalConstants.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"

BDSScreenFrame::BDSScreenFrame(){;}
BDSScreenFrame::~BDSScreenFrame(){
    delete _logVol;
}

BDSScreenFrame::BDSScreenFrame(G4String name, G4ThreeVector size, G4TwoVector windowSize, G4TwoVector windowOffset,
        G4Material* material):
_name(name),_size(size),_windowSize(windowSize),
_windowOffset(windowOffset), _material(material){;}
