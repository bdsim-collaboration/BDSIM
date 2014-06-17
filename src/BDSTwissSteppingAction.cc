/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 25.12.2003
   Copyright (c) 2003 by G.A.Blair.  ALL RIGHTS RESERVED. 
 */

//====================================================
//  Class description here ...
//
//====================================================

#include "BDSTwissSteppingAction.hh"
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh" 

#include "G4EventManager.hh"
#include "G4StackManager.hh"
#include "G4Track.hh"
#include "BDSBeamline.hh"
#include "BDSAcceleratorComponent.hh"

//====================================================

BDSTwissSteppingAction::BDSTwissSteppingAction()
{ 
  verbose           = BDSExecOptions::Instance()->GetVerbose();     
  nptwiss           = BDSExecOptions::Instance()->GetNPTwiss();

  postponedEnergy=0;
}

//====================================================

BDSTwissSteppingAction::~BDSTwissSteppingAction()
{ }


//====================================================

void BDSTwissSteppingAction::UserSteppingAction(const G4Step* ThisStep)
{ 
  // check that there actually is a next volume as it may be the end of the optics line
  if(ThisStep->GetTrack()->GetNextVolume() && ThisStep->GetTrack()->GetParentID() <= 0) 
    {
	
#ifdef DEBUG
      G4cout <<" ***"<< ThisStep->GetTrack()->GetVolume()->GetName()<<G4endl;
      G4cout <<" +++"<< ThisStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() << G4endl;      
      G4cout <<" ---"<< ThisStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() << G4endl;
      G4cout << "StepLength = " << ThisStep->GetStepLength() << G4endl;
      G4cout << "Track energy = " << ThisStep->GetTrack()->GetTotalEnergy() << G4endl;
#endif
      
      G4int curr_delim = ThisStep->GetTrack()->GetVolume()->GetName().find("_");
      G4String curr_gmad_name = ThisStep->GetTrack()->GetVolume()->GetName().substr(0,curr_delim);
      G4int delim = ThisStep->GetTrack()->GetNextVolume()->GetName().find("_");
      G4String gmad_name = ThisStep->GetTrack()->GetNextVolume()->GetName().substr(0,delim);
      if (curr_gmad_name != gmad_name && gmad_name!="World")
        { 
#ifdef DEBUG
	  G4cout << curr_gmad_name << " " << gmad_name << G4endl;
#endif
	  G4ThreeVector pos = ThisStep->GetTrack()->GetPosition();
	  postponedEnergy+=ThisStep->GetTrack()->GetTotalEnergy();
	  
	  G4StackManager* SM = G4EventManager::GetEventManager()->GetStackManager();
#ifdef DEBUG 
          G4cout << "Postponing track " << SM->GetNPostponedTrack() << G4endl;
#endif
	  if(SM->GetNPostponedTrack()!= nptwiss-1 ) { 
	    // postpone track and save its coordinates for twiss fit
	    ThisStep->GetTrack()->SetTrackStatus(fPostponeToNextEvent);
          }

	  // all tracks arrived - do rescaling
	  if(SM->GetNPostponedTrack()== nptwiss-1)
	    {
	      SM->TransferStackedTracks(fPostpone, fUrgent);
	      if(verbose) G4cout << "\nMean Energy: " << (postponedEnergy/nptwiss)/CLHEP::GeV << G4endl;

	      std::list<BDSAcceleratorComponent*>::const_iterator iBeam;
	      G4String type="";	      
	      for(BDSBeamline::Instance()->first();!BDSBeamline::Instance()->isDone();BDSBeamline::Instance()->next())
		{ 
		  if( BDSBeamline::Instance()->currentItem()->GetName() == gmad_name)
		    {
		      type = BDSBeamline::Instance()->currentItem()->GetType();
		      if(verbose) G4cout << "Next Element is: " << BDSBeamline::Instance()->currentItem()->GetName() << G4endl;
		      if(verbose) G4cout << "Element Type: " << type << G4endl;
		      G4double old_P0 = BDSGlobalConstants::Instance()->GetBeamTotalEnergy();
		      G4double old_brho = 
			sqrt(pow(old_P0,2)- pow(CLHEP::electron_mass_c2,2))/(0.299792458 * (CLHEP::GeV/(CLHEP::tesla*CLHEP::m)));
		      G4double new_P0 = postponedEnergy/nptwiss;
		      G4double new_brho = 
			sqrt(pow(new_P0,2)- pow(CLHEP::electron_mass_c2,2))/(0.299792458 * (CLHEP::GeV/(CLHEP::tesla*CLHEP::m)));
		      
		      if(BDSGlobalConstants::Instance()->GetSynchRescale()) 
			{
			  BDSBeamline::Instance()->currentItem()->SynchRescale(new_brho/old_brho);
			  if(verbose) G4cout << "Rescaling " << BDSBeamline::Instance()->currentItem()->GetName() << "by: " << new_brho/old_brho << G4endl;
			  G4cout << "*";
			  G4cout.flush();
			}
		      break;
		    }
		}
	      postponedEnergy=0;
	    }
	  return;
	}
    }
}
  
//====================================================
