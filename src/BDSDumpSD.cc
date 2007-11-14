//  
//   BDSIM, (C) 2001-2007
//    
//   version 0.4 
//   last modified : 11 Oct 2007 by malton@pp.rhul.ac.uk
//  


//
//    beam dumper/reader for online exchange - Sensitive Detector
//

const int DEBUG = 0;

#include "BDSGlobalConstants.hh" // must be first in include list

#include "BDSDumpSD.hh"
#include "BDSSamplerHit.hh"
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
#include <vector>

#include "G4SDManager.hh"

extern G4double initial_x,initial_xp,initial_y,initial_yp,initial_z,initial_E;


BDSDumpSD::BDSDumpSD(G4String name, G4String type)
  :G4VSensitiveDetector(name),StoreHit(true),itsType(type)
{
  //itsCollectionName="Dump_"+type;
  //collectionName.insert(itsCollectionName);  
}

BDSDumpSD::~BDSDumpSD()
{;}

void BDSDumpSD::Initialize(G4HCofThisEvent*HCE)
{
  //DumpCollection = 
  //  new BDSDumpHitsCollection(SensitiveDetectorName,itsCollectionName);
}

G4bool BDSDumpSD::ProcessHits(G4Step*aStep,G4TouchableHistory*ROhist)
{
  G4Track* theTrack=aStep->GetTrack();
 
  // postpone the track
  if(theTrack->GetParentID() == 0){
    if(DEBUG) G4cout<<"Dump: postponing track..."<<G4endl;
    BDSGlobals->setWaitingForDump(true);
    theTrack->SetTrackStatus(fPostponeToNextEvent);

    G4AffineTransform tf(aStep->GetPreStepPoint()->GetTouchable()->
				GetHistory()->GetTopTransform().Inverse());
    BDSGlobals->SetDumpTransform(tf);
  }
  return true;
}

void BDSDumpSD::EndOfEvent(G4HCofThisEvent*HCE)
{

}

void BDSDumpSD::clear(){} 

void BDSDumpSD::DrawAll(){} 

void BDSDumpSD::PrintAll(){} 

