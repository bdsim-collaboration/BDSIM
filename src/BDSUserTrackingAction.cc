/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#include "BDSGlobalConstants.hh"

#include "BDSUserTrackingAction.hh"
#include "G4TrackingManager.hh"
#include "G4Track.hh"

//#include "BDSNeutronTrackInfo.hh"

void BDSUserTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
  
  // store muon trajectories
  if(BDSGlobalConstants::Instance()->GetStoreMuonTrajectories())
    {
#ifdef BDSDEBUG 
      G4cout<<"STORING MUON TRAJECTORIES"<<G4endl;
#endif
      if( abs(aTrack->GetDefinition()->GetPDGEncoding())==13)
	{ fpTrackingManager->SetStoreTrajectory(true); }
      else
	{ fpTrackingManager->SetStoreTrajectory(false); }
    }

  // store neutron trajectories
  if(BDSGlobalConstants::Instance()->GetStoreNeutronTrajectories())
    {
#ifdef BDSDEBUG 
      G4cout<<"STORING NEUTRON TRAJECTORIES"<<G4endl;
#endif
      if( abs(aTrack->GetDefinition()->GetPDGEncoding())==2112)
	{ fpTrackingManager->SetStoreTrajectory(true); }
      else
	{ fpTrackingManager->SetStoreTrajectory(false); }
    }
    
    
  // store trajectories for primaries
  if(BDSGlobalConstants::Instance()->GetStoreTrajectory())
    { 
#ifdef BDSDEBUG 
      G4cout<<"STORING PRIMARY TRAJECTORIES"<<G4endl;
#endif
      if(aTrack->GetParentID()==0)
	{ fpTrackingManager->SetStoreTrajectory(true); }
      else
	{ fpTrackingManager->SetStoreTrajectory(false); }
    }
 

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


