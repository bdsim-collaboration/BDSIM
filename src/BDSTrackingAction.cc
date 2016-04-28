#include "BDSGlobalConstants.hh"
#include "BDSTrackingAction.hh"
#include "BDSTrajectory.hh"
#include "BDSDebug.hh"

#include "globals.hh" // geant4 types / globals
#include "G4TrackingManager.hh"
#include "G4Track.hh"

//#include "BDSNeutronTrackInfo.hh"

BDSTrackingAction::BDSTrackingAction():
  interactive(false)
{;}

BDSTrackingAction::BDSTrackingAction(G4bool batchMode):
  interactive(!batchMode)
{;}

void BDSTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  //G4cout << __METHOD_NAME__ << " TrackID=" << track->GetTrackID() << " ParentID=" << track->GetParentID() << G4endl;

  if (interactive || (track->GetParentID() == 0))
  {
    fpTrackingManager->SetStoreTrajectory(1);
    BDSTrajectory *bdsTraj = new BDSTrajectory(track);
    fpTrackingManager->SetTrajectory(bdsTraj);
    //G4cout << *bdsTraj << G4endl;
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
