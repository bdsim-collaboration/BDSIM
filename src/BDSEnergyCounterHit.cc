/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#include "BDSEnergyCounterHit.hh"
#include "G4ios.hh"

G4Allocator<BDSEnergyCounterHit> BDSEnergyCounterHitAllocator;

BDSEnergyCounterHit::BDSEnergyCounterHit(G4int nCopy, G4double Energy, G4double x, G4double y, G4double z, G4String name, G4int partID, G4double weight, G4bool precisionRegion, G4int TurnNumber):
  itsEnergy(Energy*weight), itsEnergyWeightedX(x*Energy*weight), itsEnergyWeightedY(y*Energy*weight), itsEnergyWeightedZ(z*Energy*weight),
  itsX(x), itsY(y), itsZ(z),
  itsWeight(weight),
  itsCopyNumber(nCopy),
  itsPartID(partID),
  itsName(name),
  itsPrecisionRegion(precisionRegion),
  itsTurnNumber(TurnNumber)
{
  /*
  G4cout << "BDSEnergyCounterHit> E = " << itsEnergy << G4endl;
  G4cout << "BDSEnergyCounterHit> x = " << itsX << G4endl;
  G4cout << "BDSEnergyCounterHit> y = " << itsY << G4endl;
  G4cout << "BDSEnergyCounterHit> z = " << itsZ << G4endl;
  G4cout << "BDSEnergyCounterHit> vol = " << itsVolumeName << G4endl;
  */
}

BDSEnergyCounterHit::~BDSEnergyCounterHit()
{;}
