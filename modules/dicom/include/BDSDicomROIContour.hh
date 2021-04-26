//
// Created by strangesyd on 3/7/21.
//

#ifndef BDSIM_BDSDICOMROICONTOUR_HH
#define BDSIM_BDSDICOMROICONTOUR_HH

#include "dcmtk/dcmdata/dcfilefo.h"
#include "G4ThreeVector.hh"
#include <iostream>
#include <vector>

class BDSDicomROIContour
{
public:
    BDSDicomROIContour();
    ~BDSDicomROIContour(){};

public:
    void AddImageIUID( OFString ima ) {
        theImageIUIDs.push_back(ima);
    }
    OFString GetGeomType() const {
        return theGeomType;
    }
    void SetGeomType( OFString gt ) {
        theGeomType = gt;
    }
    std::vector<G4ThreeVector> GetPoints() const {
        return thePoints;
    }
    std::vector<G4ThreeVector> GetDirections() {
        return theDirections;
    }
    G4double GetZ();
    void SetData( std::vector<G4ThreeVector> data );

    void AddPoints( std::vector<G4ThreeVector> points );

    void Print(std::ostream& out);

private:
    std::vector<OFString> theImageIUIDs;
    // ?  std::vector<OFString> theImageCUIDs;
    OFString theGeomType;
    std::vector<G4ThreeVector> thePoints;
    std::vector<G4ThreeVector> theDirections;

};

#endif //BDSIM_BDSDICOMROICONTOUR_HH
