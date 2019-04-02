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

#ifndef BDSSCORERINFO_H
#define BDSSCORERINFO_H

#include "BDSScorerType.hh"
#include "globals.hh"         // geant4 types / globals

#include "CLHEP/Units/SystemOfUnits.h"
#include "G4ParticleDefinition.hh"


class BDSScorerInfo
{
public:
    /// Constructor
    BDSScorerInfo(BDSScorerType scorerTypeIn,
                  G4String      nameIn,
                  G4ParticleDefinition*   particleIn,
                  G4double      minimumEnergyIn = 0*CLHEP::GeV,
                  G4double      maximumEnergyIn = 100*CLHEP::TeV,
                  G4String filenameIn="");

    BDSScorerType scorerType; ///< Scorer type.
    G4String name;     ///< Scorer name.
    G4ParticleDefinition*   particle;      ///< Particle filter
    G4double minimumEnergy;       ///< Minimal energy
    G4double maximumEnergy;       ///< Maximal energy
    G4String filename;            ///< Name of the conversion factor file


private:
    /// private default constructor as unused
    BDSScorerInfo();
};

#endif
