/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 25.12.2003
   Copyright (c) 2003 by G.A.Blair.  ALL RIGHTS RESERVED. 
 
   Modified 22.03.05 by J.C.Carter, Royal Holloway, Univ. of London.
   Added code for GABs BeamGasPlug
   Changed Killer to kill just electrons/positrons if called KILLERE

   Modified 10.05.05 IA
   Removed Killer element, added cuts per element check
   Can have killer functionality if cuts set to 0

*/


//====================================================
//  Class description here ...
//
//====================================================

#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh" 

#include "BDSSteppingAction.hh"
#include "BDSBeamline.hh"
#include <iostream>
#include "G4Track.hh"

#include"G4TransportationManager.hh"

#include"G4StepPoint.hh"
#include "G4RotationMatrix.hh"
#include "G4AffineTransform.hh"

#include "BDSMaterials.hh"
#include "G4Material.hh"
#include "G4VSensitiveDetector.hh"
#include "Randomize.hh"

#include "G4UserLimits.hh"

#include "BDSNeutronTrackInfo.hh"

#include "G4VUserTrackInformation.hh"

#include "G4VProcess.hh"

#include "G4MagneticField.hh"
#include "G4EventManager.hh"
#include "G4StackManager.hh"
#include "G4ChordFinder.hh"
#include "G4MagIntegratorDriver.hh"
#include "G4Region.hh"
#include "BDSAcceleratorComponent.hh"

#include "BDSQuadStepper.hh"
#include "BDSSextStepper.hh"
#include "BDSOctStepper.hh"
#include "myQuadStepper.hh"

#include "G4Timer.hh"

extern BDSMaterials* theMaterials;

extern G4double BDS_Threshold_Energy;
extern G4double BDSLocalRadiusOfCurvature;

extern G4int event_number;

extern G4double initial_x,initial_xp,initial_y,initial_yp,initial_z,initial_E;

//static G4LogicalVolume* LastLogVol;
//====================================================

BDSSteppingAction::BDSSteppingAction()
{ 
  verbose           = BDSExecOptions::Instance()->GetVerbose();     
  verboseStep       = BDSExecOptions::Instance()->GetVerboseStep();     
  verboseEvent      = BDSExecOptions::Instance()->GetVerboseEvent();     
  verboseEventNumber= BDSExecOptions::Instance()->GetVerboseEventNumber();
  nptwiss           = BDSExecOptions::Instance()->GetNPTwiss();

  //  itsZposTolerance=1.e-11*m;
  itsZposTolerance=1.e-4*m;
  itsPosKick=1.e-11*m;
  itsNmax=10000;
  postponedEnergy=0;
  SetTrackLength(0);
  SetTrackLengthInWorldRegion(0);
}

//====================================================

BDSSteppingAction::~BDSSteppingAction()
{ }


//====================================================
G4double BDSSteppingAction::GetTrackLengthInWorldRegion(){
  return trackLengthInWorldRegion;
}

G4double BDSSteppingAction::GetTrackLength(){
  return trackLength;
}

void BDSSteppingAction::SetTrackLengthInWorldRegion(G4double dvalue){
  trackLengthInWorldRegion = dvalue;
}

void BDSSteppingAction::SetTrackLength(G4double dvalue){
  trackLength = dvalue;
}

//====================================================

extern G4double htot;

void BDSSteppingAction::UserSteppingAction(const G4Step* ThisStep)
{ 

  // check that there actually is a next volume as it may be the end of the optics line
  if(BDSGlobalConstants::Instance()->DoTwiss()){
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
	      if(verbose) G4cout << "\nMean Energy: " << (postponedEnergy/nptwiss)/GeV << G4endl;

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
			sqrt(pow(old_P0,2)- pow(electron_mass_c2,2))/(0.299792458 * (GeV/(tesla*m)));
		      G4double new_P0 = postponedEnergy/nptwiss;
		      G4double new_brho = 
			sqrt(pow(new_P0,2)- pow(electron_mass_c2,2))/(0.299792458 * (GeV/(tesla*m)));
		      
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
  
  // ------------  output in case of verbose step ---------------------


  if((verboseStep || verboseEventNumber == event_number) && (!BDSGlobalConstants::Instance()->GetSynchRescale()) )
    {
	int ID=ThisStep->GetTrack()->GetTrackID();
	int G4precision = G4cout.precision();
	G4cout.precision(10);
	G4cout<<"This volume="<< ThisStep->GetTrack()->GetVolume()->GetName()<<G4endl;
	
	G4LogicalVolume* LogVol=ThisStep->GetTrack()->GetVolume()->GetLogicalVolume();
	G4cout<<"This log volume="<<LogVol->GetName() <<G4endl;
	
	G4cout<<"ID="<<ID<<" part="<<
	  ThisStep->GetTrack()->GetDefinition()->GetParticleName()<<
	  "Energy="<<ThisStep->GetTrack()->GetTotalEnergy()/GeV<<
	  " mom Px="
	      <<ThisStep->GetTrack()->GetMomentum()[0]/GeV<<
	  " Py="<<ThisStep->GetTrack()->GetMomentum()[1]/GeV<<
	  " Pz="<<ThisStep->GetTrack()->GetMomentum()[2]/GeV<<" vol="<<
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

// -------------  kill tracks according to cuts -------------------

G4String pName=ThisStep->GetTrack()->GetDefinition()->GetParticleName();
 
#ifndef NOSTEPPERCUT

// this cuts apply to default region
 if(pName=="gamma"){
   if(ThisStep->GetTrack()->GetKineticEnergy()<BDSGlobalConstants::Instance()->GetThresholdCutPhotons())
     {
       ThisStep->GetTrack()->SetTrackStatus(fStopAndKill);
     }
 } else if(pName=="e-"||pName=="e+"){
   if(ThisStep->GetTrack()->GetKineticEnergy()<BDSGlobalConstants::Instance()->GetThresholdCutCharged())
     {
       ThisStep->GetTrack()->SetTrackStatus(fStopAndKill);
     }
 }
#endif

 //Kill all neutrinos
 G4bool killNeutrinos = true;
 if( killNeutrinos ){
   if( pName=="nu_e" || pName=="nu_mu" || pName=="nu_tau" || pName=="anti_nu_e" || pName=="anti_nu_mu" || pName=="anti_nu_tau" ){
     ThisStep->GetTrack()->SetTrackStatus(fStopAndKill);
   }
 }
}





//====================================================




