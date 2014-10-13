/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSEnergyCounterSD_h
#define BDSEnergyCounterSD_h 1

#include "G4VSensitiveDetector.hh"
#include "BDSEnergyCounterHit.hh"
#include "G4Navigator.hh"
#include "G4GFlashSpot.hh"
#include "G4VGFlashSensitiveDetector.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class BDSEnergyCounterSD : public G4VSensitiveDetector, public G4VGFlashSensitiveDetector
{

public:
  BDSEnergyCounterSD(G4String name);
  ~BDSEnergyCounterSD();

  void Initialize(G4HCofThisEvent*HCE);
  G4bool ProcessHits(G4Step*aStep,G4TouchableHistory*ROhist);
  G4bool ProcessHits(G4GFlashSpot*aSpot ,G4TouchableHistory* ROhist);
  void EndOfEvent(G4HCofThisEvent*HCE);
  void clear();
  void DrawAll();
  void PrintAll();
  G4double GetSPositionOfStep(G4Step* aStep);

  G4int itsHCID;

private:
  /// assignment and copy constructor not implemented nor used
  BDSEnergyCounterSD& operator=(const BDSEnergyCounterSD&);
  BDSEnergyCounterSD(BDSEnergyCounterSD&);

  G4bool   verbose;
  G4String itsName;
  BDSEnergyCounterHitsCollection *BDSEnergyCounterCollection;
  G4int*   HitID;
  G4double enrg;
  G4double xpos;
  G4double ypos;
  G4double zpos;
  G4double spos;
};



#endif

