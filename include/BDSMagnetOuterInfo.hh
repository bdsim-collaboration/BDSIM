/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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
#ifndef BDSMAGNETOUTERINFO_H
#define BDSMAGNETOUTERINFO_H

#include "BDSMagnetGeometryType.hh"

#include "globals.hh"         // geant4 types / globals

#include <ostream>

class G4Material;

/**
 * @brief Holder struct of all information required to
 * create the outer geometry of a magnet. 
 * 
 * This reduces the number of argument to all magnet constructors 
 * plus aggregates common tasks in the component factory.
 * 
 * @author Laurie Nevay
 */

class BDSMagnetOuterInfo
{
public:
  /// default constructor
  BDSMagnetOuterInfo();

  /// extra constructor to assign all members at once
  BDSMagnetOuterInfo(G4String              nameIn,
		     BDSMagnetGeometryType geometryTypeIn,
		     G4double              outerDiameterIn,
		     G4Material*           outerMaterialIn,
		     G4double              angleInIn             = 0,
		     G4double              angleOutIn            = 0,
		     G4bool                yokeOnLeft            = false,
		     G4bool                buildEndPiecesIn      = true,
		     G4String              geometryTypeAndPathIn = "");

  G4String              name;
  BDSMagnetGeometryType geometryType;
  G4double              outerDiameter;
  G4Material*           outerMaterial;
  G4double              angleIn;
  G4double              angleOut;
  G4bool                yokeOnLeft;
  G4bool                buildEndPieces;
  G4String              geometryTypeAndPath;

  /// Output stream operator implementation.
  friend std::ostream& operator<< (std::ostream& out, BDSMagnetOuterInfo const& info);

};

#endif
