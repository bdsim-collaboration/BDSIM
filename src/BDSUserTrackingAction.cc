/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#include "BDSGlobalConstants.hh"

#include "BDSUserTrackingAction.hh"
#include "BDSTrajectory.hh"
#include "G4TrackingManager.hh"
#include "G4Track.hh"

//#include "BDSNeutronTrackInfo.hh"

void BDSUserTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
  //  fpTrackingManager->SetStoreTrajectory(true);  //Need to store trajectories to get the position of last scatter etc.
  //  fpTrackingManager->SetTrajectory(new BDSTrajectory(aTrack));  
  /*
    if(aTrack->GetDefinition()->GetParticleName()=="neutron")
    {
    BDSNeutronTrackInfo* Info= new BDSNeutronTrackInfo();
    Info->SetIsLogged(false);
    fpTrackingManager->SetUserTrackInformation(Info);
    }
  */
  
}

BDSUserTrackingAction::~BDSUserTrackingAction()
{;}


