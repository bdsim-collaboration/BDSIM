#include "BDSGlobalConstants.hh"
#include "BDSTrackingAction.hh"
#include "BDSTrajectory.hh"

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
  if (interactive || (track->GetParentID() == 0))
    {
      fpTrackingManager->SetStoreTrajectory(1);
      fpTrackingManager->SetTrajectory(new BDSTrajectory(track));
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
