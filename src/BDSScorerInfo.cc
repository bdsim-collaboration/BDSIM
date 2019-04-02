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
#include "BDSScorerInfo.hh"
#include "BDSScorerType.hh"

#include "globals.hh"

BDSScorerInfo::BDSScorerInfo(BDSScorerType scorerTypeIn,
                             G4String      nameIn,
                             G4ParticleDefinition*   particleIn,
                             G4double      minimumEnergyIn,
                             G4double      maximumEnergyIn,
                             G4String      filenameIn):
        scorerType(scorerTypeIn),
        name(nameIn),
        particle(particleIn),
        minimumEnergy(minimumEnergyIn),
        maximumEnergy(maximumEnergyIn),
        filename(filenameIn)
{;}