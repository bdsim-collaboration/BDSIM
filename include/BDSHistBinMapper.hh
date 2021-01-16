/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2021.

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
#ifndef BDSIM_BDSHISTBINMAPPER_H
#define BDSIM_BDSHISTBINMAPPER_H

#include "BDSBHTypedefs.hh"
#include "G4ThreeVector.hh"


class BDSHistBinMapper
{
public:
    BDSHistBinMapper(G4int nBinsIIn,G4int nBinsJIn,G4int nBinsKIn, G4int nBinsLIn, boost_histogram_axes_variant energyAxisIn);

    virtual ~BDSHistBinMapper(){;}

    virtual G4int GlobalFromIJKLIndex(G4int iIndex,
                                     G4int jIndex,
                                     G4int kIndex,
                                     G4int lIndex = 0) const;

    virtual void IJKLFromGlobal(G4int  globalBin,
                               G4int& iIndex,
                               G4int& jIndex,
                               G4int& kIndex,
                               G4int& lIndex) const;

    inline G4double NBinsI() const {return nBinsI;}
    inline G4double NBinsJ() const {return nBinsJ;}
    inline G4double NBinsK() const {return nBinsK;}
    inline G4double NBinsL() const {return nBinsL;}
    inline boost_histogram_axes_variant  GetEnergyAxis() const {return energyAxis;}

protected:
    G4int nBinsI;
    G4int nBinsJ;
    G4int nBinsK;
    G4int nBinsL;
    boost_histogram_axes_variant energyAxis;
};

#endif
