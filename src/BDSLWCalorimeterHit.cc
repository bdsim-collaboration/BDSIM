/* BDSIM code for LW Calorimeter.    Version 1.0
   Author: John Carter, Royal Holloway, Univ. of London.
   Last modified 26.7.2004
   Copyright (c) 2004 by J.C.Carter.  ALL RIGHTS RESERVED. 
*/

#include "BDSLWCalorimeterHit.hh"
#include "G4ios.hh"
#include "G4VVisManager.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"

G4Allocator<BDSLWCalorimeterHit> BDSLWCalorimeterHitAllocator;

BDSLWCalorimeterHit::BDSLWCalorimeterHit(G4int nCopy, G4double anEnergy,G4int nEvent)
  :itsEnergy(anEnergy),itsCopyNumber(nCopy),itsEventNo(nEvent)
{;}

BDSLWCalorimeterHit::~BDSLWCalorimeterHit()
{;}

BDSLWCalorimeterHit::BDSLWCalorimeterHit(const BDSLWCalorimeterHit &right):G4VHit()
{
  itsEnergy = right.itsEnergy;
  itsCopyNumber = right.itsCopyNumber;
  itsEventNo = right.itsEventNo;
}

const BDSLWCalorimeterHit& BDSLWCalorimeterHit::operator=(const BDSLWCalorimeterHit &right)
{
  itsEnergy = right.itsEnergy;
  itsCopyNumber = right.itsCopyNumber;
  itsEventNo = right.itsEventNo;
  return *this;
}


