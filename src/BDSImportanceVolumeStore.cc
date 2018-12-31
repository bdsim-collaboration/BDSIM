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
#include "BDSImportanceVolumeStore.hh"
#include <sstream>

#include "G4VPhysicalVolume.hh"


BDSImportanceVolumeStore::BDSImportanceVolumeStore(){}


BDSImportanceVolumeStore::~BDSImportanceVolumeStore(){}


void BDSImportanceVolumeStore::AddPVolume(const G4GeometryCell &cell){

  BDSSetGeometryCell::iterator it =
          fSetGeometryCell.find(cell);
  if (it != fSetGeometryCell.end()) {
    G4cout << "BDSImportanceVolumeStore::AddPVolume: cell already stored"
           << G4endl;
    return;
  }

  fSetGeometryCell.insert(cell);
}

const G4VPhysicalVolume *BDSImportanceVolumeStore::GetPVolume(const G4String &name) const {
  const G4VPhysicalVolume *pvol = 0;
  for (BDSSetGeometryCell::const_iterator it = fSetGeometryCell.begin();
       it != fSetGeometryCell.end(); ++it) {
    const G4VPhysicalVolume &vol = it->GetPhysicalVolume();
    if (vol.GetName() == name) {
      pvol =  &vol;
    }
  }
  if (!pvol) {
    G4cout << "BDSImportanceVolumeStore::GetPVolume: no physical volume named: "
           << name << ", found" << G4endl;
  }
  return pvol;
}

G4String BDSImportanceVolumeStore::GetPNames() const {
  G4String NameString;
  for (BDSSetGeometryCell::const_iterator it = fSetGeometryCell.begin();
       it != fSetGeometryCell.end(); ++it) {
    const G4VPhysicalVolume &vol = it->GetPhysicalVolume();
    std::ostringstream os;
    os << vol.GetName() << "_" << it->GetReplicaNumber()
       << "\n";
    G4String cellname = os.str();

    NameString += cellname;
  }
  return NameString;
}
