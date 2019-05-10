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

#include "BDSScorerVolumeFilter.hh"
#include "BDSAcceleratorModel.hh"
#include "G4Step.hh"
#include "G4UnitsTable.hh"


BDSScorerVolumeFilter::BDSScorerVolumeFilter(G4String name)
        :G4VSDFilter(name)
{
    worldLV = BDSAcceleratorModel::Instance()->WorldPV()->GetLogicalVolume();
}

BDSScorerVolumeFilter::~BDSScorerVolumeFilter()
{;}

G4bool BDSScorerVolumeFilter::Accept(const G4Step* aStep) const
{
    const G4Step* realWorldStep = aStep->GetTrack()->GetStep(); // Get the step in the mass world

    G4LogicalVolume* volumeLV = realWorldStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume();

    if(volumeLV == worldLV) {
        return TRUE;
    }
    return FALSE;
}