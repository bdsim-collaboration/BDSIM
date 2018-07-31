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

#include <ostream>

BDSParticleCoords::BDSParticleCoords():
  x(0),
  y(0),
  z(0),
  xp(0),
  yp(0),
  zp(0),
  t(0),
  energy(0),
  weight(1)
{;}

BDSParticleCoords::BDSParticleCoords(G4double xIn,
				     G4double yIn,
				     G4double zIn,
				     G4double xpIn,
				     G4double ypIn,
				     G4double zpIn,
				     G4double tIn,
				     G4double energyIn,
				     G4double weightIn):
  x(xIn),
  y(yIn),
  z(zIn),
  xp(xpIn),
  yp(ypIn),
  zp(zpIn),
  t(tIn),
  energy(energyIn),
  weight(weightIn)
{;}

BDSParticleCoords::BDSParticleCoords(G4ThreeVector pos,
				     G4ThreeVector mom,
				     G4double      tIn,
				     G4double      energyIn,
				     G4double      weightIn):
  x(pos.x()),
  y(pos.y()),
  z(pos.z()),
  xp(mom.x()),
  yp(mom.y()),
  zp(mom.z()),
  t(tIn),
  energy(energyIn),
  weight(weightIn)
{;}

std::ostream& operator<< (std::ostream& out, BDSParticleCoords const& p)
{
  out << "Position: (" << x  << ", " << y  << ", " << z  << ")" << G4endl;
  out << "Momentum: (" << xp << ", " << yp << ", " << zp << ")" << G4endl;
  out << "E (total): " << energy << ", t: " << t << ", weight: " << weight << G4endl;
  return out;
}
