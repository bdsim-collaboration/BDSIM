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
#ifndef BDSCRYSTALPOSITION_H
#define BDSCRYSTALPOSITION_H

#include "globals.hh" // geant4 globals / types

#include "BDSTypeSafeEnum.hh"

#include <map>

/**
 * @brief Possible position of crystal inside collimator.
 * 
 * @author Laurie Nevay
 */

/// these must be manually duplicated in the
/// corresponding source file for this header in a string map!
struct crystalpositions_def {
  enum type {left, right, both};
};

typedef BDSTypeSafeEnum<crystalpositions_def,int> BDSCrystalPosition;

#endif
