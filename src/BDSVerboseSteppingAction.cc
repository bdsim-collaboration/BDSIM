/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 25.12.2003
   Copyright (c) 2003 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

//====================================================
//  Class description here ...
//
//====================================================

#include "BDSVerboseSteppingAction.hh"

#include "G4AffineTransform.hh"
#include "G4NavigationHistory.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"

//====================================================

BDSVerboseSteppingAction::BDSVerboseSteppingAction()
{ 
}

//====================================================

BDSVerboseSteppingAction::~BDSVerboseSteppingAction()
{}

//====================================================

void BDSVerboseSteppingAction::UserSteppingAction(const G4Step* ThisStep)
{ 
  // ------------  output in case of verbose step ---------------------

  int ID=ThisStep->GetTrack()->GetTrackID();
  int G4precision = G4cout.precision();
  G4cout.precision(10);
  G4cout<<"This volume="<< ThisStep->GetTrack()->GetVolume()->GetName()<<G4endl;
	
  G4LogicalVolume* LogVol=ThisStep->GetTrack()->GetVolume()->GetLogicalVolume();
  G4cout<<"This log volume="<<LogVol->GetName() <<G4endl;
	
  G4cout<<"ID="<<ID<<" part="<<
    ThisStep->GetTrack()->GetDefinition()->GetParticleName()<<
    "Energy="<<ThisStep->GetTrack()->GetTotalEnergy()/CLHEP::GeV<<
    " mom Px="
	<<ThisStep->GetTrack()->GetMomentum()[0]/CLHEP::GeV<<
    " Py="<<ThisStep->GetTrack()->GetMomentum()[1]/CLHEP::GeV<<
    " Pz="<<ThisStep->GetTrack()->GetMomentum()[2]/CLHEP::GeV<<" vol="<<
    ThisStep->GetTrack()->GetVolume()->GetName()<<G4endl;
	
  G4cout<<" Global Position="<<ThisStep->GetTrack()->GetPosition()<<G4endl;
  G4AffineTransform tf(ThisStep->GetPreStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform());
  G4cout<<" Local Position="<< tf.TransformPoint(ThisStep->GetTrack()->GetPosition()) <<G4endl;

  if(ThisStep->GetTrack()->GetMaterial()->GetName() !="LCVacuum")
    G4cout<<"material="<<ThisStep->GetTrack()->GetMaterial()->GetName()<<G4endl;

  G4VProcess* proc=(G4VProcess*)(ThisStep->GetPostStepPoint()->
				 GetProcessDefinedStep() );

  if(proc)G4cout<<" post-step process="<<proc->GetProcessName()<<G4endl<<G4endl;


  proc=(G4VProcess*)(ThisStep->GetPreStepPoint()->
		     GetProcessDefinedStep() );

  if(proc)G4cout<<" pre-step process="<<proc->GetProcessName()<<G4endl<<G4endl;

  // set precision back
  G4cout.precision(G4precision);

}
  
//====================================================
