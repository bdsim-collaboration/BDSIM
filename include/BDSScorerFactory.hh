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

#include "globals.hh"
#include "G4String.hh"
#include "G4VPrimitiveScorer.hh"

#ifndef BDSSCORERFACTORY_H
#define BDSSCORERFACTORY_H

class BDSScorer;
class BDSScorerInfo;

class G4Material;

/**
 * @brief Interface to create any RF cavity geometry.
 *
 * @author Laurie Nevay
 */

class BDSScorerFactory
{
public:
    static BDSScorerFactory* Instance(); ///< Singleton accessor.

    /// Reset static singleton instance to nullptr and delete the singleton
    /// factories for different beam pipe styles. Delete them here as they
    /// should only be accessed through this class.
    ~BDSScorerFactory();

    /// Main function to create a piece of cavity geometry.
    G4VPrimitiveScorer* CreateScorer(const BDSScorerInfo* info);

private:
    BDSScorerFactory(); ///< Private constructor as singleton pattern.

    static BDSScorerFactory* instance; ///< Singleton instance.

    ///@{ Unused default constructors
    BDSScorerFactory(const BDSScorerFactory&) = delete;
    BDSScorerFactory& operator=(const BDSScorerFactory&) = delete;
    ///@}
    G4VPrimitiveScorer* GetAppropriateScorer(G4String name, const BDSScorerType scorerType, G4String filename);

};

#endif
