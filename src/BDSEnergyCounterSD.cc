/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#include "BDSEnergyCounterHit.hh"
#include "BDSEnergyCounterSD.hh"
#include "BDSExecOptions.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"

#include "G4AffineTransform.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4ios.hh"
#include "G4LogicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4RotationMatrix.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VTouchable.hh"

#define NMAXCOPY 5

BDSEnergyCounterSD::BDSEnergyCounterSD(G4String name)
  :G4VSensitiveDetector(name),
   energyCounterCollection(nullptr),
   primaryCounterCollection(nullptr),
   HCIDe(-1),
   HCIDp(-1),
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
}

BDSEnergyCounterSD::~BDSEnergyCounterSD()
{;}

void BDSEnergyCounterSD::Initialize(G4HCofThisEvent* HCE)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  energyCounterCollection = new BDSEnergyCounterHitsCollection(SensitiveDetectorName,collectionName[0]);
  if (HCIDe < 0)
    {HCIDe = G4SDManager::GetSDMpointer()->GetCollectionID(energyCounterCollection);}
  HCE->AddHitsCollection(HCIDe,energyCounterCollection);

  primaryCounterCollection = new BDSEnergyCounterHitsCollection(SensitiveDetectorName,collectionName[1]);
  if (HCIDp < 0)
    {HCIDp = G4SDManager::GetSDMpointer()->GetCollectionID(primaryCounterCollection);}
  HCE->AddHitsCollection(HCIDp,primaryCounterCollection);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "HCID primaries: " << HCIDp << G4endl;
  G4cout << __METHOD_NAME__ << "HCID energy:    " << HCIDe << G4endl;
#endif
}

G4bool BDSEnergyCounterSD::ProcessHits(G4Step*aStep, G4TouchableHistory* readOutTH)
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

  G4int nCopy = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();
  
  // Get translation and rotation of volume w.r.t the World Volume
  // get the coordinate transform from the read out geometry instead of the actual geometry
  // if it exsits, else assume on axis. The read out geometry is in accelerator s,x,y
  // coordinates along beam line axis
  G4AffineTransform tf;
  G4VPhysicalVolume* theVolume;
  if (readOutTH)
    {
      tf = readOutTH->GetHistory()->GetTopTransform();
      theVolume = readOutTH->GetVolume();
    }
  else
    {
      tf = (aStep->GetPreStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform());
      theVolume = aStep->GetPostStepPoint()->GetPhysicalVolume();
    }
  G4ThreeVector posbefore = aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector posafter  = aStep->GetPostStepPoint()->GetPosition();

  //calculate local coordinates
  G4ThreeVector posbeforelocal  = tf.TransformPoint(posbefore);
  G4ThreeVector posafterlocal   = tf.TransformPoint(posafter);

  // use the second point as the point of energy deposition
  // originally this was the mean of the pre and post step points, but
  // that appears to give uneven energy deposition about volume edges.
  // global
  X = posafter.x();
  Y = posafter.y();
  Z = posafter.z();
  // local
  x = posafterlocal.x();
  y = posafterlocal.y();
  z = posafterlocal.z();

  // get the s coordinate (central s + local z)
  BDSPhysicalVolumeInfo* theInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(theVolume);
  S = -1000; // unphysical default value to allow easy identification in output
  if (theInfo)
    {S = theInfo->GetSPos() + z;}
  
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
  if (regionName.contains((G4String)"precisionRegion"))
    {precisionRegion=true;}
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
  if (aStep->GetTrack()->GetParentID() == 0)
    {
      //create a duplicate hit in the primarycounter hits collection
      //there are usually a few - filter at end of event action
      BDSEnergyCounterHit* PCHit = new BDSEnergyCounterHit(*ECHit);
      //set the energy to be the full energy of the primary
      //just now it's the wee bit of energy deposited in that step
      G4double primaryEnergy = BDSGlobalConstants::Instance()->GetBeamKineticEnergy();
      PCHit->SetEnergy(primaryEnergy);
      primaryCounterCollection->insert(PCHit);
    }

  // this will kill all particles - both primaries and secondaries, but if it's being
  // recorded in an SD that means it's hit something, so ok
  if(BDSGlobalConstants::Instance()->GetStopTracks())
    {aStep->GetTrack()->SetTrackStatus(fStopAndKill);}
   
  return true;
}

G4bool BDSEnergyCounterSD::ProcessHits(G4GFlashSpot*aSpot, G4TouchableHistory* readOutTH)
{ 
  enrg = aSpot->GetEnergySpot()->GetEnergy();
#ifdef BDSDEBUG
  G4cout << "BDSEnergyCounterSD>gflash enrg = " << enrg << G4endl;
#endif
  if (enrg==0.) return false;

  G4VPhysicalVolume* currentVolume;
  if (readOutTH)
    {currentVolume = readOutTH->GetVolume();}
  else
    {currentVolume = aSpot->GetTouchableHandle()->GetVolume();}
  
  G4String           volName        = currentVolume->GetName();
  G4int              nCopy          = currentVolume->GetCopyNo();
  
  // Get Translation and Rotation of Sampler Volume w.r.t the World Volume
  G4AffineTransform tf = (aSpot->GetTouchableHandle()->GetHistory()->GetTopTransform());
  G4ThreeVector pos    = aSpot->GetPosition();

  //calculate local coordinates
  G4ThreeVector poslocal = tf.TransformPoint(pos);
  
  //global
  X = pos.x();
  Y = pos.y();
  Z = pos.z();
  //local
  x = poslocal.x();
  y = poslocal.y();
  z = poslocal.z();

  // get the s coordinate (central s + local z)
  BDSPhysicalVolumeInfo* theInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(currentVolume);
  G4double sCentral = -1000;
  if (theInfo)
    {sCentral = theInfo->GetSPos();}
  S = sCentral + z;
  
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
