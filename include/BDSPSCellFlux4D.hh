/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2020.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BDSIM_BDSPSCELLFLUX4D_H
#define BDSIM_BDSPSCELLFLUX4D_H

#include "G4PSCellFlux3D.hh"
#include "BDSBHTypedefs.hh"

class BDSHistBinMapper;

class BDSPSCellFlux4D : public G4PSCellFlux3D {

public:
    BDSPSCellFlux4D(const G4String &name,
                    const BDSHistBinMapper *mapperIn,
                    G4int ni = 1, G4int nj = 1, G4int nk = 1,
                    G4int depi = 2, G4int depj = 1, G4int depk = 0);

    BDSPSCellFlux4D(const G4String &name,
                    const BDSHistBinMapper *mapperIn, const G4String &unit,
                    G4int ni = 1, G4int nj = 1, G4int nk = 1,
                    G4int depi = 2, G4int depj = 1, G4int depk = 0);

    ~BDSPSCellFlux4D() override { ; }

protected:
    G4int GetIndex(G4Step *) override;

private:
    G4int fDepthi, fDepthj, fDepthk;
    const BDSHistBinMapper *mapper;
};

#endif //BDSIM_BDSPSCELLFLUX4D_H
