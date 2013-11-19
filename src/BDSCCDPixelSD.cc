#include "BDSGlobalConstants.hh" 
#include "BDSExecOptions.hh"
#include "BDSCCDPixelSD.hh"
#include "BDSCCDPixelHit.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4ParticleDefinition.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include "G4AffineTransform.hh"

#include "G4RunManager.hh"
#include <vector>

#include "G4SDManager.hh"

BDSCCDPixelSD::BDSCCDPixelSD(G4String name)
  :G4VSensitiveDetector(name),CCDPixelCollection(NULL)
{
  _collectionName="CCDPixel";
  collectionName.insert(_collectionName);
}

BDSCCDPixelSD::~BDSCCDPixelSD()
{;}

void BDSCCDPixelSD::Initialize(G4HCofThisEvent*)
{
  // Create CCDPixel hits collection
  CCDPixelCollection = new BDSCCDPixelHitsCollection(SensitiveDetectorName,_collectionName);
}

G4bool BDSCCDPixelSD::ProcessHits(G4Step*aStep,G4TouchableHistory*)
{
#ifdef DEBUG
  G4cout << __METHOD_NAME__ << "processing hits for sensitive detector name " << SensitiveDetectorName << G4endl;  
#endif
  G4Track* theTrack = aStep->GetTrack();
  //only store a hit for optical photons.
  if(theTrack->GetDefinition()->GetPDGEncoding() != 0) return false;
  G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
  //Do not store hit if the particle is not on the boundary 
  if(preStepPoint->GetStepStatus()!=fGeomBoundary) return false;
  
  G4int nCCDPixel=theTrack->GetVolume()->GetCopyNo();
  G4double weight=theTrack->GetWeight();
  
#ifdef DEBUG
  G4cout << __METHOD_NAME__ << " CCDPixel : " << SampName << G4endl;
  G4cout << __METHOD_NAME__ << " Storing hit: nCCDPixel weight" << G4endl;
  G4cout << __METHOD_NAME__ << " " << nCCDPixel <<" "  << weight << G4endl;
#endif

  BDSCCDPixelHit* pixelHit
    = new BDSCCDPixelHit(nCCDPixel, weight);
  CCDPixelCollection->insert(pixelHit);
  
#ifdef DEBUG
  G4cout << __METHOD_NAME__ << " entries in hits collection after inserting hit: " << CCDPixelCollection->entries() << G4endl;
#endif
  //The hit was stored, so the return value is "true".
  return true;
}

void BDSCCDPixelSD::EndOfEvent(G4HCofThisEvent*HCE)
{
  G4SDManager * SDman = G4SDManager::GetSDMpointer();
  G4int HCID = SDman->GetCollectionID(_collectionName);
  HCE->AddHitsCollection(HCID, CCDPixelCollection );
}

void BDSCCDPixelSD::clear(){} 

void BDSCCDPixelSD::DrawAll(){} 

void BDSCCDPixelSD::PrintAll(){} 
