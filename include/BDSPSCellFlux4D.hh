//
// Created by Eliott Ramoisiaux on 25/05/2020.
//

#ifndef BDSIM_BDSPSCELLFLUX4D_H
#define BDSIM_BDSPSCELLFLUX4D_H

#include "G4PSCellFlux3D.hh"

class BDSHistBinMapper;


class BDSPSCellFlux4D : public G4PSCellFlux3D{

public: // with description
    BDSPSCellFlux4D(G4String name, const G4double eLow, const G4double eHigh, const G4int ne, const BDSHistBinMapper* mapperIn,
    G4int ni=1,G4int nj=1, G4int nk=1, G4int nl=1,
            G4int depi=2, G4int depj=1, G4int depk=0, G4int depl=1);
    BDSPSCellFlux4D(G4String name, const G4double eLow, const G4double eHigh, const G4int ne, const BDSHistBinMapper* mapperIn, const G4String& unit,
            G4int ni=1,G4int nj=1, G4int nk=1, G4int nl=1,
            G4int depi=2, G4int depj=1, G4int depk=0, G4int depl=1);
    virtual ~BDSPSCellFlux4D(){;}

protected: // with description
    virtual G4int GetIndex(G4Step*);

private:
    G4int fDepthi, fDepthj, fDepthk, fDepthl, fNl, ne;
    G4double eLow, eHigh;

    const BDSHistBinMapper* mapper;

};


#endif //BDSIM_BDSPSCELLFLUX4D_H
