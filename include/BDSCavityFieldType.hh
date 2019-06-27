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
#ifndef BDSCAVITYFIELDTYPE_H
#define BDSCAVITYFIELDTYPE_H

#include "globals.hh" // geant4 globals / types

#include "BDSTypeSafeEnum.hh"

#include <map>

/**
 * @brief Type definition for RF cavity fields
 * 
 * @author William Shields
 */

struct cavityfieldtypes_def
{
  enum type {bdsim, mad8};
};

typedef BDSTypeSafeEnum<cavityfieldtypes_def,int> BDSCavityFieldType;

namespace BDS
{
  /// function to determine the enum type of the cavity field type (case-insensitive)
  BDSCavityFieldType DetermineCavityFieldType(G4String cavityFieldType);
}

#endif
