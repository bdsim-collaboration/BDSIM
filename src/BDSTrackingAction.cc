#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSTrackingAction.hh"
#include "BDSTrajectory.hh"

#include "globals.hh" // geant4 types / globals
#include "G4TrackingManager.hh"
#include "G4Track.hh"

//#include "BDSNeutronTrackInfo.hh"

BDSTrackingAction::BDSTrackingAction():
  interactive(false)
{
  storeTrajectory = BDSGlobalConstants::Instance()->StoreTrajectory();
}

BDSTrackingAction::BDSTrackingAction(G4bool batchMode):
  interactive(!batchMode)
{
  storeTrajectory = BDSGlobalConstants::Instance()->StoreTrajectory();
}

void BDSTrackingAction::PreUserTrackingAction(const G4Track* track)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " TrackID=" << track->GetTrackID()
	 << " ParentID=" << track->GetParentID() << G4endl;
#endif

  if (interactive || storeTrajectory || track->GetParentID() == 0)
    {
      fpTrackingManager->SetStoreTrajectory(1);
      BDSTrajectory* bdsTraj = new BDSTrajectory(track,interactive);
      fpTrackingManager->SetTrajectory(bdsTraj);
    }
  else
    {fpTrackingManager->SetStoreTrajectory(0);}
  
  /*
  if(track->GetDefinition()->ParticleName()=="neutron")
  {
    BDSNeutronTrackInfo* Info= new BDSNeutronTrackInfo();
    Info->SetIsLogged(false);
    fpTrackingManager->SetUserTrackInformation(Info);
  }
  */
}
