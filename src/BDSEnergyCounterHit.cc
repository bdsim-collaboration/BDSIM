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
#include <iostream>

using namespace std;

G4Allocator<BDSEnergyCounterHit> BDSEnergyCounterHitAllocator;

BDSEnergyCounterHit::  BDSEnergyCounterHit(G4int nCopy, G4double Energy, G4double x, G4double y, G4double z, G4String name, G4int partID, G4double weight):
  itsEnergy(Energy),itsCopyNumber(nCopy),
  itsName(name),
  itsZ(z),
  itsX(x), itsY(y),
  itsPartID(partID),
  itsWeight(weight)
{
  /*
  cout << "BDSEnergyCounterHit> E = " << itsEnergy << endl;
  cout << "BDSEnergyCounterHit> x = " << itsX << endl;
  cout << "BDSEnergyCounterHit> y = " << itsY << endl;
  cout << "BDSEnergyCounterHit> z = " << itsZ << endl;
  cout << "BDSEnergyCounterHit> vol = " << itsVolumeName << endl;
  */
}


BDSEnergyCounterHit::BDSEnergyCounterHit()
{;}


BDSEnergyCounterHit::~BDSEnergyCounterHit()
{;}

BDSEnergyCounterHit::BDSEnergyCounterHit(const BDSEnergyCounterHit &right):G4VHit()
{
  itsEnergy = right.itsEnergy;
  itsCopyNumber = right.itsCopyNumber;
  itsName = right.itsName;
  itsX = right.itsX;
  itsY = right.itsY;
  itsZ = right.itsZ;
}

const BDSEnergyCounterHit& BDSEnergyCounterHit::operator=(const BDSEnergyCounterHit &right)
{
  itsEnergy = right.itsEnergy;
  itsCopyNumber = right.itsCopyNumber;
  itsName = right.itsName;
  itsX = right.itsX;
  itsY = right.itsY;
  itsZ = right.itsZ;
  return *this;
}


