/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

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
#ifndef BDSIMPORTANCEVOLUMESTORE_H
#define BDSIMPORTANCEVOLUMESTORE_H

#include "globals.hh" // geant4 globals / types
#include <map>
#include <vector>

/**
 * @brief
 *
 *
 * 
 * @author Will Shields
 */

#include "globals.hh"
#include <set>
#include "G4GeometryCell.hh"
#include "G4GeometryCellComp.hh"

typedef std::set< G4GeometryCell, G4GeometryCellComp > BDSSetGeometryCell;

class BDSImportanceVolumeStore {
public:
    BDSImportanceVolumeStore();
    ~BDSImportanceVolumeStore();

    void AddPVolume(const G4GeometryCell &cell);
    const G4VPhysicalVolume *GetPVolume(const G4String &name) const;
    G4String GetPNames() const;

private:
    BDSSetGeometryCell fSetGeometryCell;
};

#endif
