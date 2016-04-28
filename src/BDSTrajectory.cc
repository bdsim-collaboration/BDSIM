#include "BDSDebug.hh"
#include "BDSTrajectory.hh"
#include "BDSTrajectoryPoint.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Allocator.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"

#include <map>
#include <ostream>

G4Allocator<BDSTrajectory> bdsTrajectoryAllocator;

BDSTrajectory::BDSTrajectory(const G4Track* aTrack): G4Trajectory(aTrack)
{
  fpBDSPointsContainer = new BDSTrajectoryPointsContainer;
}

void BDSTrajectory::AppendStep(const G4Step* aStep)
{
  G4Trajectory::AppendStep(aStep);
  fpBDSPointsContainer->push_back(new BDSTrajectoryPoint(aStep));
}

void BDSTrajectory::MergeTrajectory(G4VTrajectory* secondTrajectory)
{
  G4cout << __METHOD_NAME__ << G4endl;

  if(!secondTrajectory)
  {return;}

  G4Trajectory::MergeTrajectory(secondTrajectory);
}

std::ostream& operator<< (std::ostream& out, BDSTrajectory const& t)
{
  for(G4int i = 0; i < t.GetPointEntries(); i++)
   {out << *(BDSTrajectoryPoint*)t.GetPoint(i) << G4endl;}
  return out;
}

