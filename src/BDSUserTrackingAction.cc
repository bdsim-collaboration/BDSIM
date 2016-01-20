#include "BDSGlobalConstants.hh"
#include "BDSUserTrackingAction.hh"
#include "BDSTrajectory.hh"

#include "G4TrackingManager.hh"
#include "G4Track.hh"

//#include "BDSNeutronTrackInfo.hh"

void BDSUserTrackingAction::PreUserTrackingAction(const G4Track* /*aTrack*/)
{
  fpTrackingManager->SetStoreTrajectory(true);  //Need to store trajectories to get the position of last scatter etc.
  
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


