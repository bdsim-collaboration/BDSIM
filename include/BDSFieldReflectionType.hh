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
#ifndef BDSFIELDREFLECTIONTYPE_H
#define BDSFIELDREFLECTIONTYPE_H

#include "BDSTypeSafeEnum.hh"
#include "G4String.hh"

/**
 * @brief Type definition for field reflections
 *
 * @author Laurie Nevay
 */

struct fieldreflectiontypes_def
{
  enum type {none,
    vertical,
    horizontal,
    mirrorvertical,
    mirrorhorizontal,
    quadrantdipole,
    quadrantquadrupole};
};

typedef BDSTypeSafeEnum<fieldreflectiontypes_def,int> BDSFieldReflectionType;

namespace BDS
{
  /// Function that gives corresponding enum value for string (case-insensitive)
  BDSFieldReflectionType DetermineFieldReflectionType(G4String fieldReflectionType);
}

#endif


