//  
//   BDSIM, (C) 2001-2007
//    
//   version 0.5 
//   last modified : 12 Mar 2009 by malton@pp.rhul.ac.uk
//  


//
//    beam dumper/reader for online exchange - Sensitive Detector
//

#include "BDSGlobalConstants.hh" 

#include "BDSDumpSD.hh"
#include "BDSDump.hh"
#include "BDSSamplerHit.hh"
#include "BDSDebug.hh"
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


BDSDumpSD::BDSDumpSD(G4String name, G4String type):
  G4VSensitiveDetector(name),
  itsType(type)
{
}

BDSDumpSD::~BDSDumpSD()
{;}

void BDSDumpSD::Initialize(G4HCofThisEvent*)
{
}

G4bool BDSDumpSD::ProcessHits(G4Step*aStep,G4TouchableHistory*)
{
  G4cout << __METHOD_NAME__  <<G4endl;
  G4Track* theTrack=aStep->GetTrack();
  G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
  
  // postpone the track
  //  if(theTrack->GetParentID() == 0){
  //Check if the particle is on the boundary 
  if(preStepPoint->GetStepStatus()==fGeomBoundary){
    if(BDSGlobalConstants::Instance()->isReference){
      G4double referenceTime = theTrack->GetGlobalTime();
#ifdef DEBUG 
      G4cout << __METHOD_NAME__  << "refTime= " << referenceTime <<G4endl;
      G4cout << __METHOD_NAME__  << theTrack->GetVolume()->GetName() << G4endl;
#endif
      if(lastVolume!=theTrack->GetVolume()->GetName())
	BDSGlobalConstants::Instance()->referenceQueue.at(nCounter++)[trackCounter] = referenceTime;
      
#ifdef DEBUG 
      G4cout << __METHOD_NAME__  << "Track: " << trackCounter
	     << " Dump : " << nCounter 
	     << " Time: " << referenceTime << G4endl;
#endif
      
      if(nCounter==BDSDump::GetNumberOfDumps()){
	nCounter=0;
	++trackCounter;
	lastVolume="";
      }
      
      lastVolume = theTrack->GetVolume()->GetName();
      //    theTrack->SetGlobalTime(0);
    }
    else if(abs(theTrack->GetDefinition()->GetPDGEncoding()) == 11){
#ifdef DEBUG
      G4cout << __METHOD_NAME__ <<"Dump: postponing track..."<<G4endl;
#endif
      BDSGlobalConstants::Instance()->setWaitingForDump(true);
      //Don't postpone to next event if this is the last event.
      //      if(G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID()!=(BDSGlobalConstants::Instance()->GetNumberToGenerate()-1)){
	theTrack->SetTrackStatus(fPostponeToNextEvent);
	//      }
      
      G4AffineTransform tf(aStep->GetPreStepPoint()->GetTouchableHandle()->
			   GetHistory()->GetTopTransform().Inverse());
      BDSGlobalConstants::Instance()->SetDumpTransform(tf);
    }
  }
  return true;
}

void BDSDumpSD::EndOfEvent(G4HCofThisEvent*)
{
}

void BDSDumpSD::clear(){} 

void BDSDumpSD::DrawAll(){} 

void BDSDumpSD::PrintAll(){} 

G4int BDSDumpSD::nCounter = 0; 

G4int BDSDumpSD::trackCounter = 0; 

G4String BDSDumpSD::lastVolume = "";


