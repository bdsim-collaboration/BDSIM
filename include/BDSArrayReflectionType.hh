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
#ifndef BDSARRAYREFLECTIONTYPE_H
#define BDSARRAYREFLECTIONTYPE_H
#include "BDSTypeSafeEnum.hh"
#include "G4String.hh"

/**
 * @brief Type definition for array reflections
 *
 * @author Laurie Nevay
 */

struct arrayreflectiontypes_def
{
  enum type {none,
             flipx, flipy, flipz, flipt,
             reflectnone,
             reflectx, reflecty, reflectz, reflectt,
             reflectxydipole,
             reflectxzdipole,
             reflectyzdipole,
             reflectxyquadrupole};
};

typedef BDSTypeSafeEnum<arrayreflectiontypes_def,int> BDSArrayReflectionType;

namespace BDS
{
  /// Function that gives corresponding enum value for string (case-insensitive)
  BDSArrayReflectionType DetermineArrayReflectionType(G4String arrayReflectionType);
}

#endif


