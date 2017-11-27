/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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

/*
BDSEnergyCounterHit* BDS::LowestSPosPrimaryHit(BDSEnergyCounterHitsCollection* hc)
{
  G4double sposMin = 1e20; //stupidly large number - spos will always be less than
  G4double spos = 0;
  G4int indexofHit = -1;
  for (G4int i = 0; i < hc->entries(); ++i)
    {
      spos = (*hc)[i]->GetSBefore();
      if (spos < sposMin) {
	      sposMin = spos; //keep for testing
	      indexofHit = i; //record which hit it was
      }
    }
  if (indexofHit != -1)
    {return (*hc)[indexofHit];}
  else
    {return nullptr;}
}


BDSEnergyCounterHit* BDS::HighestSPosPrimaryHit(BDSEnergyCounterHitsCollection* hc)
{
  G4double sposMax = -1e10; //negative number - spos will always be greater than
  G4double spos = 0;
  G4int indexofHit = -1;
  for (G4int i = 0; i < hc->entries(); ++i)
    {
      spos = (*hc)[i]->GetSAfter();
      if (spos > sposMax) {
	      sposMax = spos; //keep for testing
	      indexofHit = i; //record which hit it was
      }
    }
  if (indexofHit != -1)
    {return (*hc)[indexofHit];}
  else
    {return nullptr;}
}
*/
