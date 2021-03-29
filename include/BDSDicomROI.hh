//
// Created by strangesyd on 3/7/21.
//

#ifndef BDSIM_BDSDICOMROI_HH
#define BDSIM_BDSDICOMROI_HH

#include "dcmtk/dcmdata/dcfilefo.h"

#include <vector>
#include <iostream>
#include "BDSDicomROIContour.hh"

class BDSDicomROI
{
public:
    BDSDicomROI(int ROINumber, OFString ROIName);
    ~BDSDicomROI(){};

public:
    void AddContour( BDSDicomROIContour* );
    std::vector<BDSDicomROIContour*> GetContours() const {
        return theContours;
    }
    void Print(std::ostream& out);
    int GetNumber() const {
        return theNumber;
    }
    OFString GetName() const {
        return theName;
    }

private:
    int theNumber;
    OFString theName;

    std::vector<BDSDicomROIContour*> theContours;

};

#endif //BDSIM_BDSDICOMROI_HH
