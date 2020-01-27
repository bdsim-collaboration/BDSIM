/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#ifndef BDSPARTICLEEXTERNAL_H
#define BDSPARTICLEEXTERNAL_H 
#include "BDSParticleCoords.hh"

class BDSParticleDefinition;

/**
 * @brief A set of particle coordinates from an external interface.
 * 
 * @author Laurie Nevay
 */

class BDSParticleExternal
{
public:
  BDSParticleExternal() = delete;
  BDSParticleExternal(const BDSParticleDefinition* particleDefinitionIn,
		      const BDSParticleCoords&     coordsIn);
  ~BDSParticleExternal();

  const BDSParticleDefinition* particleDefinition;
  BDSParticleCoords            coords;
};

#endif
