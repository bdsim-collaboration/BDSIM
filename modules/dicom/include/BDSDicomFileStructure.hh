//
// Created by strangesyd on 3/7/21.
//

#ifndef BDSIM_BDSDICOMFILESTRUCTURE_HH
#define BDSIM_BDSDICOMFILESTRUCTURE_HH

#include "BDSDicomVFile.hh"
#include "BDSDicomROI.hh"
class DcmDataSet;

class BDSDicomFileStructure: public BDSDicomVFile
{
public:
    BDSDicomFileStructure(DcmDataset* dset);
    virtual ~BDSDicomFileStructure(){};

public:
    std::vector<BDSDicomROI*> GetROIs() const {
        return theROIs;
    }
    virtual void ReadData();

private:
    std::vector<BDSDicomROI*> theROIs;
};

#endif //BDSIM_BDSDICOMFILESTRUCTURE_HH
