#include "BDSDebug.hh"
#include "BDSTrajectory.hh"
#include "BDSTrajectoryPoint.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Allocator.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4TrajectoryContainer.hh"
#include "G4TransportationProcessType.hh"


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
  weight                = aTrack->GetWeight();
}

BDSTrajectory::~BDSTrajectory()
{
  // clean points container
  for (auto i : fpBDSPointsContainer)
    {
      delete i;
    }
}

void BDSTrajectory::AppendStep(const G4Step* aStep)
{
  fpBDSPointsContainer.push_back(new BDSTrajectoryPoint(aStep));
}

void BDSTrajectory::MergeTrajectory(G4VTrajectory* secondTrajectory)
{
  G4cout << __METHOD_NAME__ << G4endl;

  if(!secondTrajectory)
  {return;}

  G4Trajectory::MergeTrajectory(secondTrajectory);
}

BDSTrajectoryPoint* BDSTrajectory::FirstInteraction(G4TrajectoryContainer *trajCont)
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

  // loop over primary trajectory to find non transportation step
  for(G4int i=0;i<primary->GetPointEntries();++i)
  {
    BDSTrajectoryPoint *point = dynamic_cast<BDSTrajectoryPoint*>(primary->GetPoint(i));
    if((point->GetPostProcessType()  != fTransportation) &&
       (point->GetPostProcessType() != fGeneral          && point->GetPostProcessSubType() != STEP_LIMITER))
    {
      return point;
    };
  }

  G4cout << "no interaction" << G4endl;
  return dynamic_cast<BDSTrajectoryPoint*>(primary->GetPoint(0));
}


BDSTrajectoryPoint* BDSTrajectory::LastInteraction(G4TrajectoryContainer *trajCont)
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

  // loop over primary trajectory to find non transportation step
  for(G4int i=primary->GetPointEntries()-1;i>=0 ;--i)
  {
    BDSTrajectoryPoint *point = dynamic_cast<BDSTrajectoryPoint*>(primary->GetPoint(i));
    if((point->GetPostProcessType()  != fTransportation) &&
        (point->GetPostProcessType() != fGeneral         && point->GetPostProcessSubType() != STEP_LIMITER))
    {
      return point;
    };
  }
  G4cout << "no interaction" << G4endl;
  return dynamic_cast<BDSTrajectoryPoint*>(primary->GetPoint(primary->GetPointEntries()-1));

}

std::ostream& operator<< (std::ostream& out, BDSTrajectory const& t)
{
  for(G4int i = 0; i < t.GetPointEntries(); i++)
   {out << *(BDSTrajectoryPoint*)t.GetPoint(i) << G4endl;}
  return out;
}

