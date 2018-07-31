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

BDSParticleCoordsFull::BDSParticleCoordsFull():
  local(BDSParticleCoords()),
  global(BDSParticleCoords()),
  totalEnergy(0),
  weight(1)
{;}

BDSParticleCoordsFull::BDSParticleCoordsFull(const BDSParticleCoords& localIn,
					     const BDSParticleCoords& globalIn,
					     G4double                 totalEnergyIn,
					     G4double                 weightIn):
  local(localIn),
  global(globalIn),
  totalEnergy(totalEnergyIn),
  weight(weightIn)
{;}

std::ostream& operator<< (std::ostream& out, BDSParticleCoordsFull const& p)
{
  out << "Local: " << p.local << "Global: " << p.global << G4endl;
  out << "E (total): " << p.totalEnergy << ", weight: " << p.weight << G4endl;
  return out;
}
