//
// Created by strangesyd on 3/7/21.
//

#ifndef BDSIM_BDSDICOMVFILE_HH
#define BDSIM_BDSDICOMVFILE_HH

#include <vector>
#include "globals.hh"

#include "dcmtk/dcmdata/dcfilefo.h"

class DcmDataset;

class BDSDicomVFile
{
public:
    BDSDicomVFile(){};
    BDSDicomVFile(DcmDataset* dset);
    ~BDSDicomVFile(){};

    void SetFileName( G4String fName ) {
        fFileName = fName;
    }

protected:
    virtual void ReadData() = 0;

    virtual std::vector<G4double> Read1Data( DcmDataset * dset, DcmTagKey tagKey, G4int nData );
    virtual OFString Read1DataStr( DcmDataset * dset, DcmTagKey tagKey );

    DcmDataset * theDataset;

protected:
    G4String fFileName;

};

#endif //BDSIM_BDSDICOMVFILE_HH
