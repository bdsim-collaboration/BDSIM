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

  // G4String pName=ThisStep->GetTrack()->GetDefinition()->GetParticleName();
  // better with PDG numbers (faster)
  G4int pdgNr = ThisStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding();

  //#ifndef NOSTEPPERCUT

  // this cuts apply to default region
  //  if(pName=="gamma"){
  if (pdgNr == 22) {
    if(ThisStep->GetTrack()->GetKineticEnergy()<BDSGlobalConstants::Instance()->GetThresholdCutPhotons())
      {
	ThisStep->GetTrack()->SetTrackStatus(fStopAndKill);
      }
    //  } else if(pName=="e-"||pName=="e+"){
  } else if (abs(pdgNr) == 11) {
    if(ThisStep->GetTrack()->GetKineticEnergy()<BDSGlobalConstants::Instance()->GetThresholdCutCharged())
      {
	ThisStep->GetTrack()->SetTrackStatus(fStopAndKill);
      }
  }
  //#endif
}
  
//====================================================
