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

#include "G4AffineTransform.hh"

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
  
 
  //Check if the particle is on the boundary 
  if(preStepPoint->GetStepStatus()==fGeomBoundary){
    if(abs(theTrack->GetDefinition()->GetPDGEncoding()) == 11){
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ <<"Dump: postponing track..."<<G4endl;
#endif
      BDSGlobalConstants::Instance()->setWaitingForDump(true);
      //Don't postpone to next event if this is the last event.
      //      if(BDSRunManager::GetRunManager()->GetCurrentEvent()->GetEventID()!=(BDSGlobalConstants::Instance()->GetNumberToGenerate()-1)){
	theTrack->SetTrackStatus(fPostponeToNextEvent);
	//      }
      
      G4AffineTransform tf(aStep->GetPreStepPoint()->GetTouchableHandle()->
			   GetHistory()->GetTopTransform().Inverse());
      BDSGlobalConstants::Instance()->SetDumpTransform(tf);
    }
  }
  return true;
}
