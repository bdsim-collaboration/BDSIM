/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#include "BDSEnergyCounterHit.hh"
#include "G4ios.hh"

G4Allocator<BDSEnergyCounterHit> BDSEnergyCounterHitAllocator;

BDSEnergyCounterHit::BDSEnergyCounterHit(G4int nCopy, 
					 G4double Energy, 
					 G4double x, 
					 G4double y, 
					 G4double z,
					 G4double s,
					 G4String name, 
					 G4int partID, 
					 G4double weight, 
					 G4bool precisionRegion, 
					 G4int TurnsTaken
					 ):
  itsEnergy(Energy*weight), 
  itsEnergyWeightedX(x*Energy*weight), 
  itsEnergyWeightedY(y*Energy*weight), 
  itsEnergyWeightedZ(z*Energy*weight),
  itsEnergyWeightedS(s*Energy*weight),
  itsX(x), 
  itsY(y), 
  itsZ(z),
  itsS(s),
  itsWeight(weight),
  itsCopyNumber(nCopy),
  itsPartID(partID),
  itsName(name),
  itsPrecisionRegion(precisionRegion),
  itsTurnsTaken(TurnsTaken)
{}

BDSEnergyCounterHit::~BDSEnergyCounterHit()
{;}

BDSEnergyCounterHit* BDS::FindFirstPrimaryHit(BDSEnergyCounterHitsCollection* hc)
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
  return (*hc)[indexofHit];
}

