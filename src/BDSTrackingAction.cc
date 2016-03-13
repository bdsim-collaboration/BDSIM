#include "BDSGlobalConstants.hh"
#include "BDSTrackingAction.hh"
#include "BDSTrajectory.hh"

#include "G4TrackingManager.hh"
#include "G4Track.hh"

//#include "BDSNeutronTrackInfo.hh"

BDSTrackingAction::BDSTrackingAction()
{;}

void BDSTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
  // Only store full trajectory information for primary particles
  if (aTrack->GetParentID() == 0)
    {
      fpTrackingManager->SetStoreTrajectory(1);
      fpTrackingManager->SetTrajectory(new BDSTrajectory(aTrack));
    }
  else
    {fpTrackingManager->SetStoreTrajectory(0);}
  // 1 - trajectory, 2 - smooth trajectory, 3 & 4 - rich trajectory, default nothing
  
  /*
    if(aTrack->GetDefinition()->GetParticleName()=="neutron")
    {
    BDSNeutronTrackInfo* Info= new BDSNeutronTrackInfo();
    Info->SetIsLogged(false);
    fpTrackingManager->SetUserTrackInformation(Info);
    }
  */
}
