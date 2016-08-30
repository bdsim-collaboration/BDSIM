#include "BDSAuxiliaryNavigator.hh"
#include "BDSEnergyCounterHit.hh"
#include "BDSEnergyCounterSD.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4AffineTransform.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4LogicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VTouchable.hh"

BDSEnergyCounterSD::BDSEnergyCounterSD(G4String name)
  :G4VSensitiveDetector(name),
   energyCounterCollection(nullptr),
   HCIDe(-1),
   enrg(0.0),
   weight(0.0),
   X(0.0),
   Y(0.0),
   Z(0.0),
   sBefore(0.0),
   sAfter(0.0),
   x(0.0),
   y(0.0),
   z(0.0),
   stepLength(0.0),
   precisionRegion(false),
   ptype(0),
   volName(""),
   turnstaken(0),
   eventnumber(0),
   auxNavigator(new BDSAuxiliaryNavigator())
{
  verbose = BDSGlobalConstants::Instance()->Verbose();
  itsName = name;
  collectionName.insert("energy_counter");
}

BDSEnergyCounterSD::~BDSEnergyCounterSD()
{
  delete auxNavigator;
}

void BDSEnergyCounterSD::Initialize(G4HCofThisEvent* HCE)
{
  energyCounterCollection = new BDSEnergyCounterHitsCollection(SensitiveDetectorName,collectionName[0]);
  if (HCIDe < 0)
    {HCIDe = G4SDManager::GetSDMpointer()->GetCollectionID(energyCounterCollection);}
  HCE->AddHitsCollection(HCIDe,energyCounterCollection);
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "HCID energy:    " << HCIDe << G4endl;
#endif
}

G4bool BDSEnergyCounterSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  if(BDSGlobalConstants::Instance()->StopTracks())
    {enrg = (aStep->GetTrack()->GetTotalEnergy() - aStep->GetTotalEnergyDeposit());} // Why subtract the energy deposit of the step? Why not add?
  //this looks like accounting for conservation of energy when you're killing a particle
  //which may normally break energy conservation for the whole event
  //see developer guide 6.2.2...
  else
    {enrg = aStep->GetTotalEnergyDeposit();}
#ifdef BDSDEBUG
  G4cout << "BDSEnergyCounterSD> energy = " << enrg << G4endl;
#endif
  //if the energy is 0, don't do anything
  if (!BDS::IsFinite(enrg))
    {return false;}

  G4int nCopy = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();
  
  // Get translation and rotation of volume w.r.t the World Volume
  G4VPhysicalVolume* theVolume = auxNavigator->LocateGlobalPointAndSetup(aStep);

  G4ThreeVector posbefore = aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector posafter  = aStep->GetPostStepPoint()->GetPosition();

  //calculate local coordinates
  G4ThreeVector posbeforelocal  = auxNavigator->ConvertToLocal(posbefore);
  G4ThreeVector posafterlocal   = auxNavigator->ConvertToLocal(posafter);

  // use the second point as the point of energy deposition
  // originally this was the mean of the pre and post step points, but
  // that appears to give uneven energy deposition about volume edges.
  // this also gave edge effects
  // now store both SAfter (post step point) and SBefore (pre step point)
  // global
  X = posafter.x();
  Y = posafter.y();
  Z = posafter.z();
  // local
  x = posafterlocal.x();
  y = posafterlocal.y();
  z = posafterlocal.z();

  stepLength = (posafter - posbefore).mag(); 

  // get the s coordinate (central s + local z), and precision info
  BDSPhysicalVolumeInfo* theInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(theVolume);
  G4int beamlineIndex = -1;
  if (theInfo)
    {
      sAfter  = theInfo->GetSPos() + z; //z is posafterlocal.z() - saves access
      sBefore = theInfo->GetSPos() + posbeforelocal.z();
      precisionRegion = theInfo->GetPrecisionRegion();
      beamlineIndex   = theInfo->GetBeamlineIndex();
    }
  else
    {
      // need to exit as theInfo is dereferenced later
      G4cerr << "No volume info for " << theVolume->GetName() << G4endl;
      sAfter  = -1000; // unphysical default value to allow easy identification in output
      sBefore = -1000;
      precisionRegion = false;
    }

  // TBC - note the x,y,z should reflect the exact same s coordinate randomly chosen along the step
  G4double sHit = sBefore + G4UniformRand()*(sAfter - sBefore);
  
  eventnumber = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
  
  if(verbose && BDSGlobalConstants::Instance()->StopTracks())
    {
      G4cout << "BDSEnergyCounterSD: Current Volume: " 
	     << aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() 
	     << "\tEvent:  " << eventnumber 
	     << "\tEnergy: " << enrg/CLHEP::GeV 
	     << "GeV\tPosition: " << sAfter/CLHEP::m <<" m"<< G4endl;
    }
  
  weight = aStep->GetTrack()->GetWeight();
  if (weight == 0)
    {G4cerr << "Error: BDSEnergyCounterSD: weight = 0" << G4endl; exit(1);}
  ptype      = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
  trackID    = aStep->GetTrack()->GetTrackID();
  volName    = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();  
  turnstaken = BDSGlobalConstants::Instance()->TurnsTaken();
  
  //create hits and put in hits collection of the event
  BDSEnergyCounterHit* ECHit = new BDSEnergyCounterHit(nCopy,
                                                       enrg,
                                                       X, Y, Z,
                                                       sBefore,
                                                       sAfter,
                                                       sHit,
                                                       x, y, z,
                                                       volName,
                                                       ptype,
                                                       trackID,
                                                       weight,
                                                       precisionRegion,
                                                       turnstaken,
                                                       eventnumber,
                                                       stepLength,
                                                       beamlineIndex);
  
  // don't worry, won't add 0 energy tracks as filtered at top by if statement
  energyCounterCollection->insert(ECHit);

  // this will kill all particles - both primaries and secondaries, but if it's being
  // recorded in an SD that means it's hit something, so ok
  if(BDSGlobalConstants::Instance()->StopTracks())
    {aStep->GetTrack()->SetTrackStatus(fStopAndKill);}
   
  return true;
}

G4bool BDSEnergyCounterSD::ProcessHits(G4GFlashSpot* aSpot, G4TouchableHistory*)
{ 
  enrg = aSpot->GetEnergySpot()->GetEnergy();
#ifdef BDSDEBUG
  G4cout << "BDSEnergyCounterSD> gflash energy = " << enrg << G4endl;
#endif
  if (!BDS::IsFinite(enrg))
    {return false;}

  G4VPhysicalVolume* currentVolume = aSpot->GetTouchableHandle()->GetVolume();
  G4String           volName       = currentVolume->GetName();
  G4int              nCopy         = currentVolume->GetCopyNo();
  
  // Get Translation and Rotation of Sampler Volume w.r.t the World Volume
  G4ThreeVector pos    = aSpot->GetPosition();
  auxNavigator->LocateGlobalPointAndSetup(pos);

  //calculate local coordinates
  G4ThreeVector poslocal = auxNavigator->ConvertToLocal(pos);
  
  //global
  X = pos.x();
  Y = pos.y();
  Z = pos.z();
  //local
  x = poslocal.x();
  y = poslocal.y();
  z = poslocal.z();

  stepLength = 0; // no step length available for a 'spot'

  // get the s coordinate (central s + local z)
  BDSPhysicalVolumeInfo* theInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(currentVolume);
  if (theInfo)
    {
      sAfter  = theInfo->GetSPos() + z; 
      sBefore = theInfo->GetSPos() + z; // no pre/post step for spot
      precisionRegion = theInfo->GetPrecisionRegion();
    }
  else
    {
      // need to exit as theInfo is dereferenced later
      G4cerr << "No volume info for " << currentVolume << G4endl;
      sAfter  = -1000; // unphysical default value to allow easy identification in output
      sBefore = -1000;
      precisionRegion = false;
    }

  G4double sHit = sBefore + G4UniformRand()*(sAfter - sBefore);
  
  eventnumber = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();  
  weight = aSpot->GetOriginatorTrack()->GetPrimaryTrack()->GetWeight();
  if (!BDS::IsFinite(weight))
    {G4cerr << "Error: BDSEnergyCounterSD: weight = 0" << G4endl; exit(1);}
  
  ptype   = aSpot->GetOriginatorTrack()->GetPrimaryTrack()->GetDefinition()->GetPDGEncoding();
  trackID = aSpot->GetOriginatorTrack()->GetPrimaryTrack()->GetTrackID();
  turnstaken = BDSGlobalConstants::Instance()->TurnsTaken();

  if(verbose && BDSGlobalConstants::Instance()->StopTracks())
    {
      G4cout << " BDSEnergyCounterSD: Current Volume: " <<  volName 
	     << " Event: "    << eventnumber 
	     << " Energy: "   << enrg/CLHEP::GeV << " GeV"
	     << " Position: " << sAfter/CLHEP::m   << " m" 
	     << G4endl;
    }
  
  // see explanation in other processhits function
  BDSEnergyCounterHit* ECHit = new BDSEnergyCounterHit(nCopy,
						       enrg,
						       X,
						       Y,
						       Z,
						       Z /*SBefore*/,
						       Z /*SAfter*/,
						       sHit,
						       x,
						       y,
						       z,
						       volName, 
						       ptype,
						       trackID,
						       weight, 
						       0,
						       turnstaken,
						       eventnumber,
						       stepLength,
						       theInfo->GetBeamlineIndex());
  
  // don't worry, won't add 0 energy tracks as filtered at top by if statement
  energyCounterCollection->insert(ECHit);
  
  return true;
}
