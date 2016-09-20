#include "BDSTrajectory.hh"

#include "BDSDebug.hh"
#include "BDSProcessMap.hh"
#include "BDSTrajectoryPoint.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Allocator.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4TrajectoryContainer.hh"  // also provides TrajectoryVector type(def)
#include "G4TransportationProcessType.hh"

#include <map>
#include <ostream>

G4Allocator<BDSTrajectory> bdsTrajectoryAllocator;

BDSTrajectory* BDS::GetPrimaryTrajectory(G4TrajectoryContainer* trajCont)
{
  TrajectoryVector* trajVec = trajCont->GetVector();
  BDSTrajectory*    primary = nullptr;
  for (const auto iT1 : *trajVec)
    {
      BDSTrajectory* traj = (BDSTrajectory*)iT1;
      if(traj->GetParentID() == 0)
	{primary = traj; break;}
    }
  return primary;
}

BDSTrajectory::BDSTrajectory(const G4Track* aTrack):
  G4Trajectory(aTrack)
{
  const G4VProcess *proc = aTrack->GetCreatorProcess();
  if(proc)
    {
      creatorProcessType    = aTrack->GetCreatorProcess()->GetProcessType();
      creatorProcessSubType = aTrack->GetCreatorProcess()->GetProcessSubType();
    }
  else
    {
      creatorProcessType    = -1;
      creatorProcessSubType = -1;
    }
  weight = aTrack->GetWeight();

  fpBDSPointsContainer = BDSTrajectoryPointsContainer();
  // this is for the first point of the track
  fpBDSPointsContainer.push_back(new BDSTrajectoryPoint(aTrack));
}

BDSTrajectory::~BDSTrajectory()
{
  // clean points container
  for (auto i : fpBDSPointsContainer)
    {delete i;}
}

void BDSTrajectory::AppendStep(const G4Step* aStep)
{
  // we do not use G4Trajectory::AppendStep here as that would
  // duplicate position information in its own vector of positions
  // which we prevent access to be overrideing GetPoint
  fpBDSPointsContainer.push_back(new BDSTrajectoryPoint(aStep));
}

void BDSTrajectory::MergeTrajectory(G4VTrajectory* secondTrajectory)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // this checks if nullptr and we don't do anything here
  G4Trajectory::MergeTrajectory(secondTrajectory);
}

BDSTrajectoryPoint* BDSTrajectory::FirstInteraction(BDSTrajectory* trajectory)
{
  // loop over trajectory to find non transportation step
  for (G4int i=0; i < trajectory->GetPointEntries(); ++i)
    {
      BDSTrajectoryPoint* point = static_cast<BDSTrajectoryPoint*>(trajectory->GetPoint(i));
      auto processType    = point->GetPostProcessType();
      auto processSubType = point->GetPostProcessSubType();

      // test against things we want to exclude like tracking - these are not
      // points of scattering
      G4bool initialised       = processType != -1;
      G4bool notTransportation = processType != fTransportation;
      G4bool notGeneral        = (processType != fGeneral) && (processSubType != STEP_LIMITER);
      G4bool notParallel       = processType != fParallel;

      if (initialised && notTransportation && notGeneral && notParallel)
	{
#ifdef BDSDEBUG
	  G4cout << "First primary interaction point found at "
		 << point->GetPreS()/CLHEP::m
		 << " m - "
		 << BDSProcessMap::Instance()->GetProcessName(processType, processSubType) << G4endl;
#endif
	  return point;
	}
    }
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "no interaction" << G4endl;
#endif
  return static_cast<BDSTrajectoryPoint*>(trajectory->GetPoint(0));
}

BDSTrajectoryPoint* BDSTrajectory::LastInteraction(BDSTrajectory* trajectory)
{
  // loop over trajectory backwards to find non transportation step
  for (G4int i = trajectory->GetPointEntries()-1; i >= 0; --i)
  {
    BDSTrajectoryPoint* point = static_cast<BDSTrajectoryPoint*>(trajectory->GetPoint(i));
    auto processType = point->GetPostProcessType();
    auto processSubType = point->GetPostProcessSubType();

    // test against things we want to exclude like tracking - these are not
    // points of scattering
    G4bool initialised       = processType != -1;
    G4bool notTransportation = processType != fTransportation;
    G4bool notGeneral        = (processType != fGeneral) && (processSubType != STEP_LIMITER);
    G4bool notParallel       = processType != fParallel;

    if (initialised && notTransportation && notGeneral && notParallel)
    {
#ifdef BDSDEBUG
      G4cout << "First primary interaction point found at "
		 << point->GetPreS()/CLHEP::m
		 << " m - "
		 << BDSProcessMap::Instance()->GetProcessName(processType, processSubType) << G4endl;
#endif
      return point;
    }
  }
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "no interaction" << G4endl;
#endif
  return static_cast<BDSTrajectoryPoint*>(trajectory->GetPoint(trajectory->GetPointEntries()-1));
} 

std::ostream& operator<< (std::ostream& out, BDSTrajectory const& t)
{
  for(G4int i = 0; i < t.GetPointEntries(); i++)
    {out << *(BDSTrajectoryPoint*)t.GetPoint(i) << G4endl;}
  return out;
}

