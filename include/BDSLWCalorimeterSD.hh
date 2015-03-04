/* BDSIM code for LW Calorimeter.    Version 1.0
   Author: John Carter, Royal Holloway, Univ. of London.
   Last modified 26.7.2004
   Copyright (c) 2004 by J.C.Carter.  ALL RIGHTS RESERVED. 
*/
#ifndef BDSLWCalorimeterSD_h
#define BDSLWCalorimeterSD_h 1

#include "G4VSensitiveDetector.hh"
#include "BDSLWCalorimeterHit.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class BDSLWCalorimeterSD : public G4VSensitiveDetector
{

public:
  BDSLWCalorimeterSD(G4String name);
  ~BDSLWCalorimeterSD();
  
  virtual void Initialize(G4HCofThisEvent*HCE);
  virtual G4bool ProcessHits(G4Step*aStep,G4TouchableHistory*ROhist);
  virtual void EndOfEvent(G4HCofThisEvent*HCE);
  inline void AddEnergy(G4double anEnergy)
  {itsTotalEnergy+=anEnergy;}

  G4double itsTotalEnergy;
  G4int itsCopyNumber;

private:
  BDSLWCalorimeterHitsCollection *LWCalorimeterCollection;

};




#endif

