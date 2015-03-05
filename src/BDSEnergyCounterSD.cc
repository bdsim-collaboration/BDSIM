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
#include "G4AffineTransform.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4ios.hh"
#include "G4LogicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4RotationMatrix.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VTouchable.hh"

#include <map>

BDSEnergyCounterSD::BDSEnergyCounterSD(G4String name)
  :G4VSensitiveDetector(name),
   energyCounterCollection(NULL),
   primaryCounterCollection(NULL),
   enrg(0.0),
   X(0.0),
   Y(0.0),
   Z(0.0),
   S(0.0),
   x(0.0),
   y(0.0),
   z(0.0)
{
  verbose = BDSExecOptions::Instance()->GetVerbose();
  itsName = name;
  collectionName.insert("energy_counter");
  collectionName.insert("primary_counter");
  #define NMAXCOPY 5
  HitID = new G4int[NMAXCOPY];
}

BDSEnergyCounterSD::~BDSEnergyCounterSD()
{
  delete [] HitID;
}

void BDSEnergyCounterSD::Initialize(G4HCofThisEvent* HCE)
{
  static G4int HCID = -1;
  for(G4int i=0; i<NMAXCOPY;i++)HitID[i]=-1;

  energyCounterCollection = new BDSEnergyCounterHitsCollection
    (SensitiveDetectorName,collectionName[0]);
  HCID = GetCollectionID(0);
  HCE->AddHitsCollection(HCID,energyCounterCollection);

  primaryCounterCollection = new BDSEnergyCounterHitsCollection
    (SensitiveDetectorName,collectionName[1]);
  HCID = GetCollectionID(1);
  HCE->AddHitsCollection(HCID,primaryCounterCollection);
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
  G4AffineTransform tf = (aStep->GetPreStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform());
  G4ThreeVector posbefore = aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector posafter  = aStep->GetPostStepPoint()->GetPosition();
  //G4ThreeVector momDir = aStep->GetTrack()->GetMomentumDirection();

  //calculate local coordinates
  G4ThreeVector posbeforelocal  = tf.TransformPoint(posbefore);
  G4ThreeVector posafterlocal   = tf.TransformPoint(posafter);
  //G4ThreeVector LocalDirection = tf.TransformAxis(momDir);

  //G4cout << "Global Position " << pos << G4endl;
  //G4cout << "Local Position " << poslocal << G4endl << G4endl;

  //calculate mean position of step
  //global
  Y = 0.5 * (posbefore.x() + posafter.x());
  Y = 0.5 * (posbefore.y() + posafter.y());
  Z = 0.5 * (posbefore.z() + posafter.z());
  S = GetSPositionOfStep(aStep);
  //local
  x = 0.5 * (posbeforelocal.x() + posafterlocal.x());
  y = 0.5 * (posbeforelocal.y() + posafterlocal.y());
  z = 0.5 * (posbeforelocal.z() + posafterlocal.z());

  G4int event_number = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
  
  if(verbose && BDSGlobalConstants::Instance()->GetStopTracks()) 
    {
      G4cout << "BDSEnergyCounterSD: Current Volume: " 
	     << aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() 
	     << "\tEvent:  " << event_number 
	     << "\tEnergy: " << enrg/CLHEP::GeV 
	     << "GeV\tPosition: " << S/CLHEP::m <<" m"<< G4endl;
    }
  
  G4double weight = aStep->GetTrack()->GetWeight();
  if (weight == 0){
    G4cerr << "Error: BDSEnergyCounterSD: weight = 0" << G4endl;
    exit(1);
  }
  G4int    ptype      = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
  G4String volName    = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
  G4String regionName = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetRegion()->GetName();
  
  G4bool precisionRegion = false;
  if (regionName.contains((G4String)"precisionRegion")) {
    precisionRegion=true;
  }
  //G4bool precisionRegion = get this info from the logical volume in future
  
  G4int turnstaken    = BDSGlobalConstants::Instance()->GetTurnsTaken();
  
  //create hits and put in hits collection of the event
  //do analysis / output in end of event action
  BDSEnergyCounterHit* ECHit = new BDSEnergyCounterHit(nCopy,
						       enrg,
						       X,
						       Y,
						       Z,
						       S,
						       x,
						       y,
						       z,
						       volName, 
						       ptype, 
						       weight, 
						       precisionRegion,
						       turnstaken,
						       event_number
						       );
  // don't worry, won't add 0 energy tracks as filtered at top by if statement
  energyCounterCollection->insert(ECHit);
  
  //record first scatter of primary if it exists
  if (aStep->GetTrack()->GetParentID() == 0) {
    //create a duplicate hit in the primarycounter hits collection
    //there are usually a few - filter at end of event action
    BDSEnergyCounterHit* PCHit = new BDSEnergyCounterHit(*ECHit);
    //set the energy to be the full energy of the primary
    //just now it's the wee bit of energy deposited in that step
    G4double primaryEnergy = BDSGlobalConstants::Instance()->GetBeamKineticEnergy();
    PCHit->SetEnergy(primaryEnergy);
    primaryCounterCollection->insert(PCHit);
  }
  
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
  G4AffineTransform tf = (aSpot->GetTouchableHandle()->GetHistory()->GetTopTransform());
  G4ThreeVector pos    = aSpot->GetPosition();

  //calculate local coordinates
  G4ThreeVector poslocal = tf.TransformPoint(pos);
  
  //global
  Y = pos.x();
  Y = pos.y();
  Z = pos.z();
  S = GetSPositionOfSpot(aSpot);
  //local
  x = poslocal.x();
  y = poslocal.y();
  z = poslocal.z();
  
  G4int event_number = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
  
  if(verbose && BDSGlobalConstants::Instance()->GetStopTracks()) 
    {
      G4cout << " BDSEnergyCounterSD: Current Volume: " <<  volName 
	     << " Event: "    << event_number 
	     << " Energy: "   << enrg/CLHEP::GeV << " GeV"
	     << " Position: " << S/CLHEP::m   << " m" 
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
						       X,
						       Y,
						       Z,
						       S,
						       x,
						       y,
						       z,
						       volName, 
						       ptype, 
						       weight, 
						       0,
						       turnstaken,
						       event_number
						       );
  // don't worry, won't add 0 energy tracks as filtered at top by if statement
  energyCounterCollection->insert(ECHit);
  
  //record first scatter of primary if it exists
  if (aSpot->GetOriginatorTrack()->GetPrimaryTrack()->GetParentID() == 0) {
    //create a duplicate hit in the primarycounter hits collection
    //there are usually a few - filter at end of event action
    BDSEnergyCounterHit* PCHit = new BDSEnergyCounterHit(*ECHit);
    G4double primaryEnergy = BDSGlobalConstants::Instance()->GetBeamKineticEnergy();
    PCHit->SetEnergy(primaryEnergy);
    primaryCounterCollection->insert(PCHit);
  }
  
  return true;
}

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

G4double BDSEnergyCounterSD::GetSPositionOfSpot(G4GFlashSpot* aSpot)
{
  G4double thespos;
  // Get the s position along the accelerator by querying the logical volume
  // Get the logical volume from this step
  G4LogicalVolume* thevolume = aSpot->GetTouchableHandle()->GetVolume()->GetLogicalVolume();

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
    G4ThreeVector     pos = aSpot->GetPosition();
    G4AffineTransform tf  = (aSpot->GetTouchableHandle()->GetHistory()->GetTopTransform());
    G4ThreeVector localposition = tf.TransformPoint(pos);
    thespos += localposition.z();
   }
  return thespos;
}
