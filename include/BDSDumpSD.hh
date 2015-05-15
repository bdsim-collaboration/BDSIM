//  
//   BDSIM, (C) 2001-2007
//    
//   version 0.3 
//   last modified : 08 May 2007 by agapov@pp.rhul.ac.uk
//  


//
//    beam dumper/reader for online exchange - Sensitive Detector
//


#ifndef BDSDumpSD_h
#define BDSDumpSD_h 

#include "G4VSensitiveDetector.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class BDSDumpSD : public G4VSensitiveDetector
{
  
public:
  BDSDumpSD(G4String name, G4String type);
  ~BDSDumpSD();
  
  void SetType(G4String aType);
  
  void Initialize(G4HCofThisEvent*HCE);
  G4bool ProcessHits(G4Step*aStep,G4TouchableHistory*ROhist);
  void EndOfEvent(G4HCofThisEvent*HCE);
  void clear();
  void DrawAll();
  void PrintAll();
  
private:  
  G4String itsType;
  G4String itsCollectionName;
  static unsigned int nCounter;
  static unsigned int trackCounter;
  static G4String lastVolume;
};

inline void BDSDumpSD::SetType(G4String aType)
{itsType=aType;}

#endif

