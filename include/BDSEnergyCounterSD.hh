#ifndef BDSENERGYCOUNTERSD_H
#define BDSENERGYCOUNTERSD_H

#include "BDSAuxiliaryNavigator.hh"
#include "BDSEnergyCounterHit.hh"

#include "G4Navigator.hh"
#include "G4GFlashSpot.hh"
#include "G4VSensitiveDetector.hh"
#include "G4VGFlashSensitiveDetector.hh" // G4VSensitiveDetector is required before this due to missing header

class G4VProcess;
class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

/**
 * @brief Generates BDSEnergyCounterHits from step information - uses curvilinear coords.
 *
 * This class interrogates a G4Step and generates an energy deposition hit if there was
 * a change in energy. This assigns the energy deposition to a point randomly (uniformly)
 * along the step.  It also uses a BDSAuxiliaryNavigator instance to use transforms from
 * the curvilinear parallel world for curvilinear coordinates.
 */

class BDSEnergyCounterSD: public G4VSensitiveDetector, public G4VGFlashSensitiveDetector
{
public:
  BDSEnergyCounterSD(G4String name);
  virtual ~BDSEnergyCounterSD();

  virtual void Initialize(G4HCofThisEvent*HCE);
  virtual G4bool ProcessHits(G4Step*aStep,G4TouchableHistory*ROhist);
  virtual G4bool ProcessHits(G4GFlashSpot*aSpot ,G4TouchableHistory* ROhist);
  
private:
  /// assignment and copy constructor not implemented nor used
  BDSEnergyCounterSD& operator=(const BDSEnergyCounterSD&);
  BDSEnergyCounterSD(BDSEnergyCounterSD&);
  BDSEnergyCounterSD() = delete;

  G4bool   verbose;
  G4String colName; ///< Collection name.
  BDSEnergyCounterHitsCollection* energyCounterCollection;
  G4int    HCIDe;

  ///@{ per hit variable
  G4double enrg;
  G4double weight;
  G4double X,Y,Z,sBefore,sAfter; // global coordinates
  G4double x,y,z;   // local coordinates
  G4double stepLength;
  G4int    ptype;
  G4int    trackID;
  G4int    parentID;
  G4String volName;
  G4int    turnstaken;
  G4int    eventnumber;
  ///@}

  /// Navigator for checking points in read out geometry
  BDSAuxiliaryNavigator* auxNavigator;
};

#endif

