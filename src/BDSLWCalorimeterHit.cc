/* BDSIM code for LW Calorimeter.    Version 1.0
   Author: John Carter, Royal Holloway, Univ. of London.
   Last modified 26.7.2004
   Copyright (c) 2004 by J.C.Carter.  ALL RIGHTS RESERVED. 
*/

#include "BDSLWCalorimeterHit.hh"
#include "G4ios.hh"

G4Allocator<BDSLWCalorimeterHit> BDSLWCalorimeterHitAllocator;

BDSLWCalorimeterHit::BDSLWCalorimeterHit(G4int nCopy, G4double anEnergy,G4int nEvent)
  :itsEnergy(anEnergy),itsCopyNumber(nCopy),itsEventNo(nEvent)
{;}

BDSLWCalorimeterHit::~BDSLWCalorimeterHit()
{;}
