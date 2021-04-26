//
// Created by strangesyd on 3/12/21.
//

#ifndef BDSIM_BDSDICOMPHANTOMZSLICEMERGED_HH
#define BDSIM_BDSDICOMPHANTOMZSLICEMERGED_HH

#include <map>
#include "globals.hh"

#include "BDSDicomPhantomZSliceHeader.hh"

class BDSDicomPhantomZSliceMerged
{
public:
    // Constructor and Destructors
    BDSDicomPhantomZSliceMerged();
    ~BDSDicomPhantomZSliceMerged();

public:
    // Public functions
    void AddZSlice(BDSDicomPhantomZSliceHeader* val) {
        fSlices[val->GetSliceLocation()] = val;
    }

    void CheckSlices();

    inline void DumpExcessMemory();

private:
    // Private functions

private:
    // Private variables
    std::map<G4double, BDSDicomPhantomZSliceHeader*> fSlices;


};

inline void BDSDicomPhantomZSliceMerged::DumpExcessMemory()
{
    for(std::map<G4double, BDSDicomPhantomZSliceHeader*>::iterator ite = fSlices.begin();
        ite != fSlices.end(); ++ite) {
        ite->second->DumpExcessMemory();
    }
}

#endif //BDSIM_BDSDICOMPHANTOMZSLICEMERGED_HH
