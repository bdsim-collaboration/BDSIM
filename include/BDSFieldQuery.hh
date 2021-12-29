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
#ifndef BDSFIELDQUERY_H
#define BDSFIELDQUERY_H

#include "G4Transform3D.hh"
#include "G4Types.hh"

#include <vector>

class BDSFieldQueryInfo;

class BDSFieldQuery
{
public:
  BDSFieldQuery();
  ~BDSFieldQuery();
  
  void QueryFields(const std::vector<BDSFieldQueryInfo*>& fieldQueries);
  void QueryField(const BDSFieldQueryInfo* query);
  
private:
  void OpenFiles(const BDSFieldQueryInfo* query);
  void CloseFiles();
  void ApplyTransform(const G4Transform3D& globalTransform,
                      G4double xLocal, G4double yLocal, G4double zLocal, G4double tLocal,
                      G4double& xGlobal, G4double& yGlobal, G4double& zGlobal, G4double& tGlobal);
  void GetFieldValue(G4double xGlobal, G4double yGlobal, G4double zGlobal, G4double tGlobal, G4double fieldValue[6]);
  void WriteFieldValue(G4double xGlobal, G4double yGlobal, G4double zGlobal, G4double tGlobal, G4double fieldValue[6]);
};

#endif
