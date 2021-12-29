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

#include "G4AffineTransform.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <fstream>
#include <vector>

class G4Navigator;
class G4VPhysicalVolume;

class BDSFieldQueryInfo;

/**
 *  @brief Class for querying the Geant4 model for field at any point.
 *
 *  Output is a BDSIM-format field map. Unique files for electric and magnetic
 *  field maps as would be required to read the field maps back into BDSIM.
 *
 *  @author Laurie Nevay
 */
class BDSFieldQuery
{
public:
  BDSFieldQuery();
  ~BDSFieldQuery();
  
  /// Query the field in the Geant4 model according to information in query.
  void QueryField(const BDSFieldQueryInfo* query);
  /// Vector version of above function. Unique output files for each query.
  void QueryFields(const std::vector<BDSFieldQueryInfo*>& fieldQueries);
  
  /// Setup the navigator w.r.t. to a world volume.
  static void AttachWorldVolumeToNavigator(G4VPhysicalVolume* worldPVIn);
  
private:
  /// Reset any member variables used during a query. Closes any files if open.
  void CleanUp();
  
  /// Open potentially both electric and magnetic field map files.
  void OpenFiles(const BDSFieldQueryInfo* query);
  
  /// Utility to write required header information.
  void WriteHeader(std::ofstream& out, const BDSFieldQueryInfo* query) const;
  
  /// Close any files if open.
  void CloseFiles();
  
  /// Apply a transform to the coordinates. Does not apply to time.
  G4ThreeVector ApplyTransform(const G4AffineTransform& globalTransform,
                               G4double xLocal, G4double yLocal, G4double zLocal) const;
  
  /// Get the electric and magnetic field at the specified coordinates. The navigator requires
  /// a direction for safe hierarchy searching. The output is written to array argument, where
  /// the values are Bx,By,Bz,Ex,Ey,Ez as in Geant4.
  void GetFieldValue(const G4ThreeVector& globalXYZ,
                     const G4ThreeVector& globalDirection,
                     G4double tGlobal,
                     G4double fieldValue[6]);
  
  /// Write an entry ta line of the output file(s). The array is assumed to be Bx,By,Bz,Ex,Ey,Ez as in Geant4.
  void WriteFieldValue(const G4ThreeVector& xyzGlobal,
                       G4double tGlobal,
                       const G4double fieldValue[6]);
  
  std::ofstream oFileMagnetic;
  std::ofstream oFileElectric;
  G4bool queryMagnetic;
  G4bool queryElectric;
  G4bool writeX;
  G4bool writeY;
  G4bool writeZ;
  G4bool writeT;
  
  /// One navigator used - static so we can externally link it up to the world PV.
  static G4Navigator* navigator;
};

#endif
