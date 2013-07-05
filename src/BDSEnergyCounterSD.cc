/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"

#include "BDSEnergyCounterSD.hh"
#include "BDSEnergyCounterHit.hh"
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

#include "G4Navigator.hh"
#include "G4AffineTransform.hh"

#include "G4RunManager.hh"
#include "G4Version.hh"

#include <string>

extern G4int event_number;


BDSEnergyCounterSD::BDSEnergyCounterSD(G4String name)
  :G4VSensitiveDetector(name),BDSEnergyCounterCollection(NULL),
   enrg(0.0),xpos(0.0),ypos(0.0),zpos(0.0)
{
  verbose = BDSExecOptions::Instance()->GetVerbose();
  itsName = name;
  collectionName.insert("EC_"+name);
  #define NMAXCOPY 5
  HitID = new G4int[NMAXCOPY];
}

BDSEnergyCounterSD::~BDSEnergyCounterSD()
{
  delete [] HitID;
}

void BDSEnergyCounterSD::Initialize(G4HCofThisEvent*)
{
  BDSEnergyCounterCollection = new BDSEnergyCounterHitsCollection
    (SensitiveDetectorName,collectionName[0]); 
  for(G4int i=0; i<NMAXCOPY;i++)HitID[i]=-1;
}

G4bool BDSEnergyCounterSD::ProcessHits(G4Step*aStep,G4TouchableHistory*)
{ 
  if(BDSGlobalConstants::Instance()->GetStopTracks())
#if G4VERSION_NUMBER > 940
    enrg = (aStep->GetTrack()->GetTotalEnergy() - aStep->GetTotalEnergyDeposit()); // Why subtract the energy deposit of the step? Why not add?
#else
  enrg = (aStep->GetTrack()->GetTotalEnergy() - aStep->GetDeltaEnergy()); // Why subtract the energy deposit of the step? Why not add?
#endif
  else
    enrg = aStep->GetTotalEnergyDeposit();
#ifdef DEBUG
  G4cout << "BDSEnergyCounterSD> enrg = " << enrg << G4endl;
#endif
  if (enrg==0.) return false;      
  
  
  G4int nCopy=aStep->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();
#ifdef DEBUG
  if(nCopy>0){
    G4cout << "BDSEnergyCounterSD::ProcessHits> nCopy = " << nCopy << G4endl;
  }
#endif
  if(nCopy>NMAXCOPY-1)
    {
      G4cerr<<" BDSEnergyCounterSD: nCopy too large: nCopy="<<nCopy<<
	"NMAXCOPY="<<NMAXCOPY<<" Volume="<<
	aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName()<<G4endl;
      G4Exception("Killing program in BDSEnergyCounterSD::ProcessHits", "-1", FatalException, "");
    }
  
  // Get Translation and Rotation of Sampler Volume w.r.t the World Volume
  // as described in Geant4 FAQ's: http://geant4.cern.ch/support/faq.shtml
  G4AffineTransform tf = (aStep->GetPreStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform());
  G4ThreeVector pos    = aStep->GetTrack()->GetPosition();
  G4ThreeVector momDir = aStep->GetTrack()->GetMomentumDirection();

  G4ThreeVector LocalPosition  = tf.TransformPoint(pos);
  G4ThreeVector LocalDirection = tf.TransformAxis(momDir);

  //  xpos=LocalPosition.x();
  //  ypos=LocalPosition.y();
  //  zpos=LocalPosition.z();
  

  zpos=0.5*(aStep->GetPreStepPoint()->GetPosition().z()
  	    + aStep->GetPostStepPoint()->GetPosition().z());
  
  xpos=0.5*(aStep->GetPreStepPoint()->GetPosition().x()
	    + aStep->GetPostStepPoint()->GetPosition().x());
  
  ypos=0.5*(aStep->GetPreStepPoint()->GetPosition().y()
  	    + aStep->GetPostStepPoint()->GetPosition().y());
  
  if(verbose && BDSGlobalConstants::Instance()->GetStopTracks()) {
    G4cout << "BDSEnergyCounterSD: Current Volume: " << 	aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() 
	   <<"\tEvent: " << event_number << "\tEnergy: " << enrg/GeV << "GeV\tPosition: " << zpos/m <<"m"<< G4endl;
  }
  
  /*
    G4cout << "E = " << enrg << G4endl;
    G4cout << "x = " << xpos << G4endl;
    G4cout << "y = " << ypos << G4endl;
    G4cout << "z = " << zpos << G4endl;
    G4cout << "vol1 = " << aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() << G4endl;
    G4cout << "vol2 = " << aStep->GetTrack()->GetVolume()->GetName() << G4endl;
  */

  G4double weight = aStep->GetTrack()->GetWeight();
  if (weight == 0){
    G4cerr << "Error: BDSEnergyCounterSD: weight = 0" << G4endl;
    exit(1);
  }
  int ptype = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
  G4String volName = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
  G4String regionName = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetRegion()->GetName();
  G4bool precisionRegion=false;
  if (regionName == (G4String)"precisionRegion") {
    precisionRegion=true;
  }
   
   
  if ((HitID[nCopy]==-1) || precisionRegion){
    //|| (volName.contains("INTDMP"))){ 
    //if (HitID[nCopy]==-1){ 
    
    BDSEnergyCounterHit* ECHit = new BDSEnergyCounterHit(nCopy,enrg,xpos,ypos,zpos,volName, ptype, weight, precisionRegion);
    HitID[nCopy]= BDSEnergyCounterCollection->insert(ECHit)-1; 
  } else {
    //     (*BDSEnergyCounterCollection)[HitID[nCopy]]-> AddEnergy(enrg);
    //     (*BDSEnergyCounterCollection)[HitID[nCopy]]-> AddPos(xpos, ypos, zpos);
    (*BDSEnergyCounterCollection)[HitID[nCopy]]-> AddEnergyWeightedPosition(enrg, xpos, ypos, zpos, weight);
  }
  
  if(BDSGlobalConstants::Instance()->GetStopTracks())
    aStep->GetTrack()->SetTrackStatus(fStopAndKill);
  
  
  return true;
}



G4bool BDSEnergyCounterSD::ProcessHits(G4GFlashSpot *aSpot,G4TouchableHistory*)
{ 
  enrg = aSpot->GetEnergySpot()->GetEnergy();
#ifdef DEBUG
  G4cout << "BDSEnergyCounterSD>gflash enrg = " << enrg << G4endl;
#endif
  if (enrg==0.) return false;      
  G4VPhysicalVolume* pCurrentVolume = aSpot->GetTouchableHandle()->GetVolume();
  G4String volName =  pCurrentVolume->GetName();
  G4int nCopy=pCurrentVolume->GetCopyNo();
#ifdef DEBUG
  if(nCopy>0){
    G4cout << "BDSEnergyCounterSD::ProcessHits>gFlash nCopy = " << nCopy << G4endl;
  }
#endif
  if(nCopy>NMAXCOPY-1)
    {
      G4cerr<<" BDSEnergyCounterSD: nCopy too large: nCopy="<<nCopy<<
	"NMAXCOPY="<<NMAXCOPY<<" Volume="<< volName;
      
      G4Exception("Killing program in BDSEnergyCounterSD::ProcessHits", "-1", FatalException, "");
    }
  
  // Get Translation and Rotation of Sampler Volume w.r.t the World Volume
  // as described in Geant4 FAQ's: http://geant4.cern.ch/support/faq.shtml
  G4AffineTransform tf=(aSpot->GetTouchableHandle()->GetHistory()->GetTopTransform());
  G4ThreeVector pos = aSpot->GetPosition();
  G4ThreeVector LocalPosition = tf.TransformPoint(pos);

  zpos=pos.z();
  xpos=pos.x();
  ypos=pos.y();
  
  if(verbose && BDSGlobalConstants::Instance()->GetStopTracks()) G4cout << "BDSEnergyCounterSD: Current Volume: " <<  volName <<"\tEvent: " << event_number << "\tEnergy: " << enrg/GeV << "GeV\tPosition: " << zpos/m <<"m"<< G4endl;
  
  G4double weight = aSpot->GetOriginatorTrack()->GetPrimaryTrack()->GetWeight();
  if (weight == 0){
    G4cerr << "Error: BDSEnergyCounterSD: weight = 0" << G4endl;
    exit(1);
  }
  int ptype = aSpot->GetOriginatorTrack()->GetPrimaryTrack()->GetDefinition()->GetPDGEncoding();
  if (HitID[nCopy]==-1){
    BDSEnergyCounterHit* ECHit = new BDSEnergyCounterHit(nCopy,enrg,xpos,ypos,zpos,volName, ptype, weight, 0);
    HitID[nCopy]= BDSEnergyCounterCollection->insert(ECHit)-1;
  } else {
    (*BDSEnergyCounterCollection)[HitID[nCopy]]-> AddEnergyWeightedPosition(enrg, xpos, ypos, zpos, weight);
  }
  return true;
}


void BDSEnergyCounterSD::EndOfEvent(G4HCofThisEvent*HCE)
{
  //  G4int HCID = GetCollectionID(0); 
  //  HCE->AddHitsCollection( HCID, BDSEnergyCounterCollection );
  
  //  G4SDManager *SDman = G4SDManager::GetSDpointer();
  //  G4int HCID         = SDMan->GetCollectionID(itsName);
  //  HCE->AddHitsCollection( HCID, BDSEnergyCounterCollection);  
}

void BDSEnergyCounterSD::clear()
{} 

void BDSEnergyCounterSD::DrawAll()
{} 

void BDSEnergyCounterSD::PrintAll()
{} 

