/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2019.

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

#ifndef BDSIM_BDSSCORERFILTER_HH
#define BDSIM_BDSSCORERFILTER_HH

#include "globals.hh"
#include "G4VSDFilter.hh"
#include "G4SDParticleWithEnergyFilter.hh"
#include "BDSScorerInfo.hh"
#include "BDSScorerTimeFilter.hh"
#include "BDSScorerVolumeFilter.hh"

class BDSScorerFilter  : public G4VSDFilter
{

    //-------
public: // with description
    BDSScorerFilter(G4String name, const BDSScorerInfo* info);


        virtual ~BDSScorerFilter();

    public: // with description
        virtual G4bool Accept(const G4Step*) const;

    private:
    BDSScorerFilter() = delete;
    G4SDParticleWithEnergyFilter* fParticleWithKineticEnergyFilter;
    BDSScorerTimeFilter* fTimeFilter;
    BDSScorerVolumeFilter* fVolumeFilter;
};

#endif