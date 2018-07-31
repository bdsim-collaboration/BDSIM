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
#include "BDSParticleCoords.hh"
#include "BDSParticleCoordsFull.hh"

#include <ostream>

BDSParticleCoordsFullGlobal::BDSParticleCoordsFullGlobal():
  local(BDSParticleCoordsFull()),
  global(BDSParticleCOords())
{;}

BDSParticleCoordsFullGlobal::BDSParticleCoordsFullGlobal(const BDSParticleCoordsFull& localIn,
							 const BDSParticleCoords&     globalIn):
  local(localIn),
  global(globalIn)
{;}

std::ostream& operator<< (std::ostream& out, BDSParticleCoordsFullGlobal const& p)
{
  p.Print(out);
  return out;
}

void BDSParticleCoordsFull::Print(std::ostream& out) const
{
  BDSParticleCoords::Print(out);
  out << "s: " << s << ", E (total): " << totalEnergy << ", weight: " << p.weight << G4endl;
  return out;
}
