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
#include "BDSScorerType.hh"
#include "BDSDebug.hh"

#include <map>
#include <string>

// dictionary for BDSCavityType
template<>
std::map<BDSScorerType, std::string>* BDSScorerType::dictionary =
        new std::map<BDSScorerType, std::string> ({
            {BDSScorerType::deposited_dose,  "deposited_dose"},
            {BDSScorerType::deposited_energy, "deposited_energy"},
            {BDSScorerType::population, "population"},
            {BDSScorerType::ambient_dose,  "ambient_dose"}
        });


BDSScorerType BDS::DetermineScorerType(G4String scorerType)
{
    std::map<G4String, BDSScorerType> types;
    types["deposited_dose"]  = BDSScorerType::deposited_dose;
    types["deposited_energy"]  = BDSScorerType::deposited_energy;
    types["population"]  = BDSScorerType::population;
    types["ambient_dose"]  = BDSScorerType::ambient_dose;

    scorerType.toLower();

    auto result = types.find(scorerType);
    if (result == types.end())
    {
        // it's not a valid key
        G4cout << __METHOD_NAME__ << "\"" << scorerType << "\" is not a valid scorer type" << G4endl;
        G4cout << "Available scorer types are:" << G4endl;
        for (auto it : types)
        {G4cout << "\"" << it.first << "\"" << G4endl;}
        exit(1);
    }

#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << " determined scorer type to be " << result->second << G4endl;
#endif
    return result->second;
}
