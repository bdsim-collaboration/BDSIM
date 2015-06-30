/* BDSIM code for LW Calorimeter.    Version 1.0
   Author: John Carter, Royal Holloway, Univ. of London.
   Last modified 26.7.2004
   Copyright (c) 2004 by J.C.Carter.  ALL RIGHTS RESERVED. 
*/

#include "BDSGlobalConstants.hh" 

#include "BDSRunManager.hh"
#include "BDSLWCalorimeterSD.hh"
#include "BDSLWCalorimeterHit.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ParticleDefinition.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include "G4AffineTransform.hh"

#include <vector>

BDSLWCalorimeterSD::BDSLWCalorimeterSD(G4String name)
  :G4VSensitiveDetector(name),itsTotalEnergy(0.0),itsCopyNumber(-1),
   LWCalorimeterCollection(NULL)
{
  collectionName.insert("LWCalorimeterCollection");
}

BDSLWCalorimeterSD::~BDSLWCalorimeterSD()
{;}

void BDSLWCalorimeterSD::Initialize(G4HCofThisEvent*)
{
  LWCalorimeterCollection = 
    new BDSLWCalorimeterHitsCollection(SensitiveDetectorName,collectionName[0]);
  itsTotalEnergy = 0.;
}

G4bool BDSLWCalorimeterSD::ProcessHits(G4Step*aStep,G4TouchableHistory*)
{
  
  // NOTE ON COPYNUMBER: It is not possible (?) to get the copy number of the
  //                     calorimeter directly because the cal is built with 
  //                     the exact same specifications each time and so the 
  //                     last copynumber given is the copynumber for each.
  //                     Solution for now is to take the copynumber of the
  //                     mother volume (i.e. MarkerVolume). Drawback is that
  //                     it is not possible to have more than one cal per
  //                     marker volume.


  G4int motherCopyNo = aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume(1)->GetCopyNo();
  itsCopyNumber = motherCopyNo+1;
  AddEnergy(aStep->GetTotalEnergyDeposit());
  
#ifdef BDSDEBUG 
  G4cout << "Its Copy Number is: " << itsCopyNumber << G4endl; 
  G4cout << "The Volumer here is: " << aStep->GetTrack()->GetVolume()->GetName() << G4endl;
  G4cout<<"edep="<<aStep->GetTotalEnergyDeposit()/CLHEP::GeV<<"Total so far="<<itsTotalEnergy/CLHEP::GeV<< " for event: " << BDSRunManager::GetRunManager()->GetCurrentEvent()->GetEventID() << G4endl;
#endif
  return true;
  
}

void BDSLWCalorimeterSD::EndOfEvent(G4HCofThisEvent*HCE)
{
  G4int nEvent= 
    BDSRunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
  /*
  G4cout << "ITS COPY NUMBER IS: " << itsCopyNumber << G4endl;
  G4cout << "ITS TOTAL ENERGY IS: " << itsTotalEnergy << G4endl;
  G4cout << "ITS EVENT NUMBER IS: " << nEvent << G4endl;
  */
  if(itsCopyNumber > 0){
    BDSLWCalorimeterHit* smpHit=
      new BDSLWCalorimeterHit(itsCopyNumber,itsTotalEnergy,nEvent);
    
    LWCalorimeterCollection->insert(smpHit);
  }
    
  static G4int HCID = -1;
  if(HCID<0)
    { HCID = GetCollectionID(0); }
  HCE->AddHitsCollection( HCID, LWCalorimeterCollection );
}
