#include "BDSDebug.hh"
#include "BDSTrajectory.hh"
#include "BDSTrajectoryPoint.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Allocator.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4TrajectoryContainer.hh"

#include <map>
#include <ostream>

G4Allocator<BDSTrajectory> bdsTrajectoryAllocator;

BDSTrajectory::BDSTrajectory(const G4Track* aTrack): G4Trajectory(aTrack)
{
  const G4VProcess *proc = aTrack->GetCreatorProcess();
  if(proc)
  {
    creatorProcessType = aTrack->GetCreatorProcess()->GetProcessType();
    creatorProcessSubType = aTrack->GetCreatorProcess()->GetProcessSubType();
  }
  else
  {
    creatorProcessType    = -1;
    creatorProcessSubType = -1;
  }
  particleID            = aTrack->GetParticleDefinition()->GetPDGEncoding();
  initKineticEnergy     = aTrack->GetKineticEnergy();
  initMomentum          = aTrack->GetMomentum();

  fpBDSPointsContainer = new BDSTrajectoryPointsContainer;

}

void BDSTrajectory::AppendStep(const G4Step* aStep)
{
  fpBDSPointsContainer->push_back(new BDSTrajectoryPoint(aStep));
}

void BDSTrajectory::MergeTrajectory(G4VTrajectory* secondTrajectory)
{
  G4cout << __METHOD_NAME__ << G4endl;

  if(!secondTrajectory)
  {return;}

  G4Trajectory::MergeTrajectory(secondTrajectory);
}

BDSTrajectoryPoint* BDSTrajectory::FirstLoss(G4TrajectoryContainer *trajCont)
{
  TrajectoryVector* trajVec = trajCont->GetVector();
  BDSTrajectory *primary = nullptr;
  for (auto iT1 : *trajVec)
  {
    BDSTrajectory *traj = (BDSTrajectory *) (iT1);
    if(traj->GetParentID() == 0)
    {
      primary = traj;
    }
  }
  return dynamic_cast<BDSTrajectoryPoint*>(primary->GetPoint(0));
}


BDSTrajectoryPoint* BDSTrajectory::LastLoss(G4TrajectoryContainer *trajCont)
{
  TrajectoryVector* trajVec = trajCont->GetVector();
  BDSTrajectory *primary = nullptr;
  for (auto iT1 : *trajVec)
  {
    BDSTrajectory *traj = (BDSTrajectory *) (iT1);
    if(traj->GetParentID() == 0)
    {
      primary = traj;
    }
  }
  return dynamic_cast<BDSTrajectoryPoint*>(primary->GetPoint(primary->GetPointEntries()-1));
}

std::ostream& operator<< (std::ostream& out, BDSTrajectory const& t)
{
  for(G4int i = 0; i < t.GetPointEntries(); i++)
   {out << *(BDSTrajectoryPoint*)t.GetPoint(i) << G4endl;}
  return out;
}

