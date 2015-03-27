/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#include "BDSEnergyCounterHit.hh"
#include "G4ios.hh"

G4Allocator<BDSEnergyCounterHit> BDSEnergyCounterHitAllocator;

BDSEnergyCounterHit::BDSEnergyCounterHit(G4int    nCopyIn, 
					 G4double energyIn, 
					 G4double XIn, 
					 G4double YIn, 
					 G4double ZIn,
					 G4double SIn,
					 G4double xIn,
					 G4double yIn,
					 G4double zIn,
					 G4String nameIn, 
					 G4int    partIDIn, 
					 G4double weightIn, 
					 G4bool   precisionRegionIn, 
					 G4int    turnsTakenIn,
					 G4int    eventNoIn
					 ):
  energy(energyIn*weightIn), 
  energyWeightedX(XIn*energyIn*weightIn), 
  energyWeightedY(YIn*energyIn*weightIn), 
  energyWeightedZ(ZIn*energyIn*weightIn),
  energyWeightedS(SIn*energyIn*weightIn),
  X(XIn),
  Y(YIn),
  Z(ZIn),
  S(SIn),
  x(xIn),
  y(yIn),
  z(zIn),
  weight(weightIn),
  copyNumber(nCopyIn),
  partID(partIDIn),
  name(nameIn),
  precisionRegion(precisionRegionIn),
  turnsTaken(turnsTakenIn),
  eventNo(eventNoIn)
{;}

BDSEnergyCounterHit::~BDSEnergyCounterHit()
{;}

BDSEnergyCounterHit* BDS::LowestSPosPrimaryHit(BDSEnergyCounterHitsCollection* hc)
{
  G4double sposMin = 1e20; //stupidly large number - spos will always be less than
  G4double spos = 0;
  G4int indexofHit = -1;
  for (G4int i = 0; i < hc->entries(); ++i)
    {
      spos = (*hc)[i]->GetS();
      if (spos < sposMin) {
	sposMin = spos; //keep for testing
	indexofHit = i; //record which hit it was
      }
    }
  if (indexofHit != -1)
    {return (*hc)[indexofHit];}
  else
    {return NULL;}
}


BDSEnergyCounterHit* BDS::HighestSPosPrimaryHit(BDSEnergyCounterHitsCollection* hc)
{
  G4double sposMax = -1e10; //negative number - spos will always be greater than
  G4double spos = 0;
  G4int indexofHit = -1;
  for (G4int i = 0; i < hc->entries(); ++i)
    {
      spos = (*hc)[i]->GetS();
      if (spos > sposMax) {
	sposMax = spos; //keep for testing
	indexofHit = i; //record which hit it was
      }
    }
  if (indexofHit != -1)
    {return (*hc)[indexofHit];}
  else
    {return NULL;}
}
