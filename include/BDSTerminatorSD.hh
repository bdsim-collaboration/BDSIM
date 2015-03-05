/* BDSIM
   Author: L. Nevay,
   Last modified 15/04/2014
   Copyright (c) 2014.  ALL RIGHTS RESERVED.
*/

#ifndef BDSTerminatorSD_h
#define BDSTerminatorSD_h

#include "G4VSensitiveDetector.hh"

class BDSTerminatorSD: public G4VSensitiveDetector
{
public:
  BDSTerminatorSD(G4String name);
  ~BDSTerminatorSD();

  virtual void   Initialize (G4HCofThisEvent* HCE);
  virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist);
  virtual void   EndOfEvent (G4HCofThisEvent* HCE);

private:
  G4bool verbose;
};

#endif
