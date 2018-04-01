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
#include "BDSEnergyCounterHit.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Allocator.hh"

G4Allocator<BDSEnergyCounterHit> BDSEnergyCounterHitAllocator;

BDSEnergyCounterHit::BDSEnergyCounterHit(G4int    nCopyIn,
					 G4double energyIn,
					 G4double XIn, 
					 G4double YIn, 
					 G4double ZIn,
					 G4double sBeforeIn,
					 G4double sAfterIn,
					 G4double sHitIn,
					 G4double xIn,
					 G4double yIn,
					 G4double zIn,
					 G4String nameIn, 
					 G4int    partIDIn,
					 G4int    trackIDIn,
					 G4int    parentIDIn,
					 G4double weightIn, 
					 G4int    turnsTakenIn,
					 G4int    eventNoIn,
					 G4double stepLengthIn,
					 G4int    beamlineIndexIn,
                                         G4int    geomFlagIn):
  copyNumber(nCopyIn),
  energy(energyIn),
  X(XIn),
  Y(YIn),
  Z(ZIn),
  sBefore(sBeforeIn),
  sAfter(sAfterIn),
  sHit(sHitIn),
  x(xIn),
  y(yIn),
  z(zIn),
  name(nameIn),
  partID(partIDIn),
  trackID(trackIDIn),
  parentID(parentIDIn),
  weight(weightIn),
  turnsTaken(turnsTakenIn),
  eventNo(eventNoIn),
  stepLength(stepLengthIn),
  beamlineIndex(beamlineIndexIn),
  geomFlag(geomFlagIn)
{;}

BDSEnergyCounterHit::~BDSEnergyCounterHit()
{;}
