//
// Created by strangesyd on 3/7/21.
//

#ifndef BDSIM_BDSDICOMFILEPLAN_HH
#define BDSIM_BDSDICOMFILEPLAN_HH

#include "BDSDicomVFile.hh"
class DcmDataSet;

class BDSDicomFilePlan : public BDSDicomVFile
{
public:
    BDSDicomFilePlan(DcmDataset* dset);
    ~BDSDicomFilePlan(){};

public:
    virtual void ReadData();
    void CheckData0(OFString title, Sint32 val);

    void DumpToFile();

    void SetControlPointMetersets();

private:
    //std::vector<DicomBeam*> theBeams;
    //Sint32 theNumberOfBeams;
};

#endif //BDSIM_BDSDICOMFILEPLAN_HH
