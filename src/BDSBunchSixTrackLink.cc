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
#include "BDSBunchSixTrackLink.hh"
#include "BDSDebug.hh"
#include "BDSParticleDefinition.hh"

#include "globals.hh"
#include "G4String.hh"

#include <vector>

BDSBunchSixTrackLink::BDSBunchSixTrackLink():
  currentIndex(0),
  size(0)
{;}

BDSBunchSixTrackLink::~BDSBunchSixTrackLink()
{;}

BDSParticleCoordsFull BDSBunchSixTrackLink::GetNextParticleLocal()
{
  if (currentIndex >= size)
    {
      G4cout << __METHOD_NAME__ << "looping" << G4endl;
      currentIndex = 0;
    }

  G4int ci = currentIndex;
  currentIndex++;
  return particles[ci].coords;
}

void BDSBunchSixTrackLink::AddParticle(BDSParticleDefinition*       particleDefinitionIn,
                                       const BDSParticleCoordsFull& coordsIn)
{
  particles.emplace_back(BDSParticleExternal(particleDefinitionIn, coordsIn, (G4int)particles.size()));
  size = (G4int)particles.size();
  if (!particleDefinition)
    {particleDefinition = particles.back().particleDefinition;}
}
