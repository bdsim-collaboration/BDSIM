//
// Created by Eliott Ramoisiaux on 25/05/2020.
//

#ifndef BDSIM_BDSPSCELLFLUX4D_H
#define BDSIM_BDSPSCELLFLUX4D_H

#include "G4PSCellFlux3D.hh"
#include "BDSBHTypedefs.hh"

class BDSHistBinMapper;


class BDSPSCellFlux4D : public G4PSCellFlux3D{

public: // with description
    BDSPSCellFlux4D(const G4String& name, const boost_histogram_axes_variant& energyAxis, const BDSHistBinMapper* mapperIn,
    G4int ni=1,G4int nj=1, G4int nk=1,
            G4int depi=2, G4int depj=1, G4int depk=0);
    BDSPSCellFlux4D(const G4String& name, const boost_histogram_axes_variant& energyAxis, const BDSHistBinMapper* mapperIn, const G4String& unit,
            G4int ni=1,G4int nj=1, G4int nk=1,
            G4int depi=2, G4int depj=1, G4int depk=0);
    ~BDSPSCellFlux4D() override {;}

protected: // with description
    G4int GetIndex(G4Step*) override;

private:
    G4int fDepthi, fDepthj, fDepthk;
    const boost_histogram_axes_variant& energyAxis;
    const BDSHistBinMapper* mapper;
};


#endif //BDSIM_BDSPSCELLFLUX4D_H
