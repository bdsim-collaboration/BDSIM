/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#include "BDSExecOptions.hh"
#include "BDSDebug.hh"
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

//#include "G4RunManager.hh"
#include "G4SDManager.hh"

#include <map>

extern G4int event_number;


BDSEnergyCounterSD::BDSEnergyCounterSD(G4String name)
  :G4VSensitiveDetector(name),
   itsHCID(-1),
   BDSEnergyCounterCollection(NULL),
   enrg(0.0),
   xpos(0.0),
   ypos(0.0),
   zpos(0.0),
   spos(0.0)
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

void BDSEnergyCounterSD::Initialize(G4HCofThisEvent*HCE)
{
  BDSEnergyCounterCollection = new BDSEnergyCounterHitsCollection
    (SensitiveDetectorName,collectionName[0]); 
  for(G4int i=0; i<NMAXCOPY;i++)HitID[i]=-1;
  if (itsHCID < 0){
    itsHCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);}
  HCE->AddHitsCollection(itsHCID,BDSEnergyCounterCollection);
}

G4bool BDSEnergyCounterSD::ProcessHits(G4Step*aStep,G4TouchableHistory*)
{ 
  if(BDSGlobalConstants::Instance()->GetStopTracks())
    enrg = (aStep->GetTrack()->GetTotalEnergy() - aStep->GetTotalEnergyDeposit()); // Why subtract the energy deposit of the step? Why not add?
  //this looks like accounting for conservation of energy when you're killing a particle
  //which may normally break energy conservation for the whole event
  //see developer guide 6.2.2...
  else
    enrg = aStep->GetTotalEnergyDeposit();
#ifdef BDSDEBUG
  G4cout << "BDSEnergyCounterSD> enrg = " << enrg << G4endl;
#endif
  //if the energy is 0, don't do anything
  if (enrg==0.) return false;      
  
  G4int nCopy=aStep->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();
#ifdef BDSDEBUG
  if(nCopy>0){
    G4cout << "BDSEnergyCounterSD::ProcessHits> nCopy = " << nCopy << G4endl;
  }
#endif
  if(nCopy>NMAXCOPY-1)
    {
      G4cerr << " BDSEnergyCounterSD: nCopy too large: nCopy = " << nCopy 
	     << " NMAXCOPY = " << NMAXCOPY 
	     << " Volume = " << aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName()
	     << G4endl;
      G4Exception("Killing program in BDSEnergyCounterSD::ProcessHits", "-1", FatalException, "");
    }
  
  // Get Translation and Rotation of Sampler Volume w.r.t the World Volume
  // as described in Geant4 FAQ's: http://geant4.cern.ch/support/faq.shtml
  //G4AffineTransform tf = (aStep->GetPreStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform());
  G4ThreeVector pos    = aStep->GetTrack()->GetPosition();
  G4ThreeVector momDir = aStep->GetTrack()->GetMomentumDirection();

  //G4ThreeVector LocalPosition  = tf.TransformPoint(pos);
  //G4ThreeVector LocalDirection = tf.TransformAxis(momDir);

  zpos=0.5*(aStep->GetPreStepPoint()->GetPosition().z()
  	    + aStep->GetPostStepPoint()->GetPosition().z());
  
  xpos=0.5*(aStep->GetPreStepPoint()->GetPosition().x()
	    + aStep->GetPostStepPoint()->GetPosition().x());
  
  ypos=0.5*(aStep->GetPreStepPoint()->GetPosition().y()
  	    + aStep->GetPostStepPoint()->GetPosition().y());
  
  if(verbose && BDSGlobalConstants::Instance()->GetStopTracks()) 
    {
    G4cout << "BDSEnergyCounterSD: Current Volume: " 
	   << aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() 
	   << "\tEvent:  " << event_number 
	   << "\tEnergy: " << enrg/CLHEP::GeV 
	   << "GeV\tPosition: " << zpos/CLHEP::m <<" m"<< G4endl;
    }

   G4double weight = aStep->GetTrack()->GetWeight();
   if (weight == 0){
     G4cerr << "Error: BDSEnergyCounterSD: weight = 0" << G4endl;
     exit(1);
   }
   G4int    ptype      = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
   G4String volName    = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
   G4String regionName = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetRegion()->GetName();
   
   spos = GetSPositionOfStep(aStep);

   G4bool precisionRegion = false;
   if (regionName.contains((G4String)"precisionRegion")) {
     precisionRegion=true;
   }
   //G4bool precisionRegion = get this info from the logical volume in future
   
   G4int turnstaken    = BDSGlobalConstants::Instance()->GetTurnsTaken();
   
   // always create a new hit as averaging can be done at output / histogram time
   // if averaging now, no way back later
   // amount of data is manageable
   BDSEnergyCounterHit* ECHit = new BDSEnergyCounterHit(nCopy,
							enrg,
							xpos,
							ypos,
							zpos,
							spos,
							volName, 
							ptype, 
							weight, 
							precisionRegion,
							turnstaken
							);
   // don't worry, won't add 0 energy tracks as filtered at top by if statement
   BDSEnergyCounterCollection->insert(ECHit);
   
   if(BDSGlobalConstants::Instance()->GetStopTracks())
     aStep->GetTrack()->SetTrackStatus(fStopAndKill);

  return true;
}



G4bool BDSEnergyCounterSD::ProcessHits(G4GFlashSpot *aSpot,G4TouchableHistory*)
{ 
  enrg = aSpot->GetEnergySpot()->GetEnergy();
#ifdef BDSDEBUG
  G4cout << "BDSEnergyCounterSD>gflash enrg = " << enrg << G4endl;
#endif
  if (enrg==0.) return false;      
  G4VPhysicalVolume* pCurrentVolume = aSpot->GetTouchableHandle()->GetVolume();
  G4String           volName        = pCurrentVolume->GetName();
  G4int              nCopy          = pCurrentVolume->GetCopyNo();
#ifdef BDSDEBUG
  if(nCopy>0){
    G4cout << "BDSEnergyCounterSD::ProcessHits>gFlash nCopy = " << nCopy << G4endl;
  }
#endif
  if(nCopy > NMAXCOPY-1)
    {
      G4cerr << " BDSEnergyCounterSD: nCopy too large: nCopy = " << nCopy
	     << " NMAXCOPY = " << NMAXCOPY 
	     << " Volume = "<< volName;
      G4Exception("Killing program in BDSEnergyCounterSD::ProcessHits", "-1", FatalException, "");
    }
  
  // Get Translation and Rotation of Sampler Volume w.r.t the World Volume
  // as described in Geant4 FAQ's: http://geant4.cern.ch/support/faq.shtml
  G4AffineTransform tf  = (aSpot->GetTouchableHandle()->GetHistory()->GetTopTransform());
  G4ThreeVector     pos = aSpot->GetPosition();
  
  zpos=pos.z();
  xpos=pos.x();
  ypos=pos.y();
  
  // Get the s position along the accelerator by querying the logical volume
  // Get the logical volume from this step
  G4LogicalVolume* thevolume = aSpot->GetTouchableHandle()->GetVolume()->GetLogicalVolume();

  // Find it's s position from global map made at constrcution time
  typedef std::map<G4LogicalVolume*,BDSLogicalVolumeInfo*>::iterator it_type;
  it_type search = BDSGlobalConstants::Instance()->LogicalVolumeInfo()->find(thevolume);
  
  if (search == BDSGlobalConstants::Instance()->LogicalVolumeInfo()->end()){
    //this means that the logical volume pointer doesn't exist in the map 
    //checking this prevents segfaults
    spos = -1.0*CLHEP::m; // set to unreal s position to identify and not fail
  }
  else {
    spos = BDSGlobalConstants::Instance()->GetLogicalVolumeInfo(thevolume)->GetSPos();
    G4ThreeVector localposition = tf.TransformPoint(pos);
    spos += localposition.z();
  }
  
  if(verbose && BDSGlobalConstants::Instance()->GetStopTracks()) 
    {
      G4cout << " BDSEnergyCounterSD: Current Volume: " <<  volName 
	     << " Event: "    << event_number 
	     << " Energy: "   << enrg/CLHEP::GeV << " GeV"
	     << " Position: " << zpos/CLHEP::m   << " m" 
	     << G4endl;
    }
  
  G4double weight = aSpot->GetOriginatorTrack()->GetPrimaryTrack()->GetWeight();
  if (weight == 0){
    G4cerr << "Error: BDSEnergyCounterSD: weight = 0" << G4endl;
    exit(1);
  }
  int ptype = aSpot->GetOriginatorTrack()->GetPrimaryTrack()->GetDefinition()->GetPDGEncoding();

  G4int turnstaken = BDSGlobalConstants::Instance()->GetTurnsTaken();
  
  // see explanation in other processhits function
  BDSEnergyCounterHit* ECHit = new BDSEnergyCounterHit(nCopy,
						       enrg,
						       xpos,
						       ypos,
						       zpos,
						       spos,
						       volName, 
						       ptype, 
						       weight, 
						       0,
						       turnstaken
						       );
  // don't worry, won't add 0 energy tracks as filtered at top by if statement
  BDSEnergyCounterCollection->insert(ECHit);
  
  return true;
}

void BDSEnergyCounterSD::EndOfEvent(G4HCofThisEvent* /*HCE*/)
{}

void BDSEnergyCounterSD::clear()
{} 

void BDSEnergyCounterSD::DrawAll()
{} 

void BDSEnergyCounterSD::PrintAll()
{} 

G4double BDSEnergyCounterSD::GetSPositionOfStep(G4Step* aStep)
{
  G4double thespos;
  // Get the s position along the accelerator by querying the logical volume
  // Get the logical volume from this step
  G4LogicalVolume* thevolume = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume();  
  // Find it's s position from global map made at constrcution time
  typedef std::map<G4LogicalVolume*,BDSLogicalVolumeInfo*>::iterator it_type;
  it_type search = BDSGlobalConstants::Instance()->LogicalVolumeInfo()->find(thevolume);
  
  if (search == BDSGlobalConstants::Instance()->LogicalVolumeInfo()->end()){
    //this means that the logical volume pointer doesn't exist in the map 
    //checking this prevents segfaults
    thespos = -1.0*CLHEP::m; // set to unreal s position to identify and not fail
  }
  else {
    thespos = BDSGlobalConstants::Instance()->GetLogicalVolumeInfo(thevolume)->GetSPos();
    G4ThreeVector     prestepposition = aStep->GetPreStepPoint()->GetPosition();
    G4AffineTransform tf              = (aStep->GetPreStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform());
    G4ThreeVector     prestepposlocal = tf.TransformPoint(prestepposition);
    thespos += prestepposlocal.z();
   }
  return thespos;
}
