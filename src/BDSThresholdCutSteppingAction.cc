/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 25.12.2003
   Copyright (c) 2003 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

//====================================================
//  Class description here ...
//
//====================================================

#include "BDSThresholdCutSteppingAction.hh"
#include "BDSGlobalConstants.hh" 

#include "G4Track.hh"

//====================================================

BDSThresholdCutSteppingAction::BDSThresholdCutSteppingAction()
{ 
}

//====================================================

BDSThresholdCutSteppingAction::~BDSThresholdCutSteppingAction()
{}

//====================================================

void BDSThresholdCutSteppingAction::UserSteppingAction(const G4Step* ThisStep)
{ 
  // -------------  kill tracks according to cuts -------------------

  G4String pName=ThisStep->GetTrack()->GetDefinition()->GetParticleName();
 
  //#ifndef NOSTEPPERCUT

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
  //#endif

  //Kill all neutrinos - should go to BDSUserTrackingAction? - JS
  G4bool killNeutrinos = true;
  if( killNeutrinos ){
    if( pName=="nu_e" || pName=="nu_mu" || pName=="nu_tau" || pName=="anti_nu_e" || pName=="anti_nu_mu" || pName=="anti_nu_tau" ){
      ThisStep->GetTrack()->SetTrackStatus(fStopAndKill);
    }
  }
}
  
//====================================================
