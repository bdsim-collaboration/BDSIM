#ifndef BDSENERGYCOUNTERSD_H
#define BDSENERGYCOUNTERSD_H

#include "G4VSensitiveDetector.hh"
#include "BDSEnergyCounterHit.hh"
#include "G4Navigator.hh"
#include "G4GFlashSpot.hh"
#include "G4VGFlashSensitiveDetector.hh"

class G4VProcess;
class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class BDSEnergyCounterSD : public G4VSensitiveDetector, public G4VGFlashSensitiveDetector
{
public:
  BDSEnergyCounterSD(G4String name);
  virtual ~BDSEnergyCounterSD();

  virtual void Initialize(G4HCofThisEvent*HCE);
  virtual G4bool ProcessHits(G4Step*aStep,G4TouchableHistory*ROhist);
  virtual G4bool ProcessHits(G4GFlashSpot*aSpot ,G4TouchableHistory* ROhist);
  
  G4String GetName();

  /// Instantiate a G4Navigator with the read out geometry - must be run
  /// after the read out geometry has been attached to this SD!!
  void SetUpAuxilliaryNavigator();

private:
  /// assignment and copy constructor not implemented nor used
  BDSEnergyCounterSD& operator=(const BDSEnergyCounterSD&);
  BDSEnergyCounterSD(BDSEnergyCounterSD&);

  G4bool   verbose;
  G4String itsName;
  BDSEnergyCounterHitsCollection* energyCounterCollection;
  BDSEnergyCounterHitsCollection* primaryCounterCollection;
  G4int    HCIDe;
  G4int    HCIDp;

  ///@{ per hit variable
  G4double enrg;
  G4double weight;
  G4double X,Y,Z,sBefore,sAfter; // global coordinates
  G4double x,y,z;   // local coordinates
  G4double stepLength;
  G4bool   precisionRegion;
  G4int    ptype;
  G4String volName;
  G4int    turnstaken;
  G4int    eventnumber;
  ///@}

  /// Navigator for checking points in read out geometry
  G4Navigator* auxilliaryNavigator;
};

inline G4String BDSEnergyCounterSD::GetName()
{return itsName;}


#endif

