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
#ifndef BDSTRAJECTORYFILTER_H
#define BDSTRAJECTORYFILTER_H

#include "BDSTypeSafeEnum.hh"

/**
 * @brief Type definition for trajectory filters.
 *
 * Note: no Geant4 classes are used by this as it can be called by the output.
 * 
 * @author Laurie Nevay
 */

struct trajectoryfilterstypes_def
{
  // "nFilters" should always be LAST -> used to know the length
  // of the enum (no good standard way)
  enum type {primary, depth, particle, energyThreshold, sampler, elossSRange,
	     transportation, minimumZ, maximumR, nFilters};
};

typedef BDSTypeSafeEnum<trajectoryfiltertypes_def,int> BDSTrajectoryFilter;

#endif
