#ifndef BDSCCDPIXELSD_H
#define BDSCCDPIXELSD_H

#include "G4VSensitiveDetector.hh"
#include "BDSCCDPixelHit.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class BDSCCDPixelSD : public G4VSensitiveDetector
{
  
public:
  BDSCCDPixelSD(G4String name);
  ~BDSCCDPixelSD();
  
  virtual void Initialize(G4HCofThisEvent*HCE);
  virtual G4bool ProcessHits(G4Step*aStep,G4TouchableHistory*ROhist);
  virtual void EndOfEvent(G4HCofThisEvent*HCE);
  
private:
  BDSCCDPixelHitsCollection *CCDPixelCollection;
  //  G4bool                    StoreHit;
  //  G4String itsType;
  G4String _collectionName;
};
#endif

