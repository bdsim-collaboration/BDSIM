/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#include "BDSEnergyCounterHit.hh"
#include "G4ios.hh"
#include "G4VVisManager.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"

G4Allocator<BDSEnergyCounterHit> BDSEnergyCounterHitAllocator;

BDSEnergyCounterHit::BDSEnergyCounterHit(G4int nCopy, G4double anEnergy, 
    G4double EnWeightZ):itsEnergy(anEnergy),itsCopyNumber(nCopy),
         itsEnergyWeightedPosition(EnWeightZ)
{;}


BDSEnergyCounterHit::BDSEnergyCounterHit()
{;}


BDSEnergyCounterHit::~BDSEnergyCounterHit()
{;}

BDSEnergyCounterHit::BDSEnergyCounterHit(const BDSEnergyCounterHit &right)
{
  itsEnergy = right.itsEnergy;
  itsCopyNumber = right.itsCopyNumber;
}

const BDSEnergyCounterHit& BDSEnergyCounterHit::operator=(const BDSEnergyCounterHit &right)
{
  itsEnergy = right.itsEnergy;
  itsCopyNumber = right.itsCopyNumber;
  return *this;
}


