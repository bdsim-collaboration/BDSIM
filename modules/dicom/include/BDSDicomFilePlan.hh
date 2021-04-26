//
// Created by strangesyd on 3/7/21.
//

#ifndef BDSDICOMFILEPLAN_H
#define BDSDICOMFILEPLAN_H

#include "BDSDicomVFile.hh"
class DcmDataSet;

class BDSDicomFilePlan: public BDSDicomVFile
{
public:
    BDSDicomFilePlan(DcmDataset* dset);
    virtual ~BDSDicomFilePlan(){};
    
    virtual void ReadData();
    void CheckData0(OFString title, Sint32 val);

    void DumpToFile();

    void SetControlPointMetersets();

private:
    //std::vector<DicomBeam*> theBeams;
    //Sint32 theNumberOfBeams;
};

#endif
