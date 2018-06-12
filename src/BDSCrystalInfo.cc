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
#include "BDSCrystalInfo.hh"
#include "BDSCrystalType.hh"

#include "globals.hh"

BDSCrystalInfo::BDSCrystalInfo(G4Material*    materialIn,
			       G4String       dataIn,
			       BDSCrystalType crystalTypeIn,
			       G4double       lengthXIn,
			       G4double       lengthYIn,
			       G4double       lengthZIn,
			       G4double       bendingAngleYAxisIn,
			       G4double       bendingAngleZAxisIn):
  material(materialIn),
  data(dataIn),
  shape(crystalTypeIn),
  lengthX(lengthXIn),
  lengthY(lengthYIn),
  lengthZ(lengthZIn),
  bendingAngleYAxis(bendingAngleYAxisIn),
  bendingAngleZAxis(bendingAngleZAxisIn)
{;}
