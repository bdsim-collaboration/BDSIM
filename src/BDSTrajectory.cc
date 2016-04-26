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

BDSTrajectory::BDSTrajectory(const G4Track* aTrack):
  G4Trajectory(aTrack)
{
  if(aTrack->GetCreatorProcess() != 0 )
  {
    G4cout << "BDSTrajectory::BDSTrajectory> Creator " << aTrack->GetParentID() << " " << aTrack->GetTrackID() << " " << aTrack->GetCreatorProcess()->GetProcessType() << " " <<
    aTrack->GetCreatorProcess()->GetProcessSubType() << G4endl;
  }
  else
  {
    G4cout << "BDSTrajectory::BDSTrajectory> No creator " << aTrack->GetParentID() << " " << aTrack->GetTrackID() << G4endl;
  }
  positionOfLastScatter[aTrack->GetTrackID()] = aTrack->GetPosition();
  momDirAtLastScatter[aTrack->GetTrackID()]   = aTrack->GetMomentumDirection();
  energyAtLastScatter[aTrack->GetTrackID()]   = aTrack->GetTotalEnergy();
  timeAtLastScatter[aTrack->GetTrackID()]     = aTrack->GetGlobalTime();
  timeAtVertex[aTrack->GetTrackID()]          = aTrack->GetGlobalTime();
}

void BDSTrajectory::AppendStep(const G4Step* aStep)
{
  if(aStep->GetTrack()->GetTrackID() == 1)
  {
    G4cout <<  "BDSTrajector::AppendStep> " << aStep->GetPreStepPoint()->GetProcessDefinedStep();
    if(aStep->GetPreStepPoint()->GetProcessDefinedStep() != 0)
    {
      G4cout << " " << aStep->GetPreStepPoint()->GetProcessDefinedStep()->GetProcessType() << " "
      <<   aStep->GetPreStepPoint()->GetProcessDefinedStep()->GetProcessSubType() << G4endl;
    }
    else
    {
      G4cout << G4endl;
    }
  }

#if 0
  if(aStep->GetTrack()->GetCreatorProcess() != 0)
  {
    G4cout << "BDSTrajectory::AppendStep> " << aStep->GetTrack()->GetTrackID() << " " << aStep->GetTrack()->GetCreatorProcess()->GetProcessType() << " " <<
    aStep->GetTrack()->GetCreatorProcess()->GetProcessSubType()<< G4endl;
  }
  else
  {
    G4cout << "BDSTrajectory::AppendStep>" << G4endl;
  }
#endif

  G4Trajectory::AppendStep(aStep);
  G4Track*            aTrack = aStep->GetTrack();
  BDSTrajectoryPoint* tempTP = new BDSTrajectoryPoint(aStep);
  if(tempTP->IsScatteringProcess())
    {
      positionOfLastScatter[aTrack->GetTrackID()] = aTrack->GetPosition();
      momDirAtLastScatter[aTrack->GetTrackID()]   = aTrack->GetMomentumDirection();
      energyAtLastScatter[aTrack->GetTrackID()]   = aTrack->GetTotalEnergy();
      timeAtLastScatter[aTrack->GetTrackID()]     = aTrack->GetGlobalTime();
    }
  delete tempTP;
}

void BDSTrajectory::MergeTrajectory(G4VTrajectory* secondTrajectory)
{
  if(!secondTrajectory)
    {return;}

  G4Trajectory::MergeTrajectory(secondTrajectory);
  
  BDSTrajectory* seco = (BDSTrajectory*)secondTrajectory;
  for (auto position : seco->positionOfLastScatter)
    {positionOfLastScatter.insert(position);}
  seco->positionOfLastScatter.clear();
}

std::ostream& operator<< (std::ostream& out, BDSTrajectory const& t)
{
  for(G4int i = 1; i < t.GetPointEntries(); i++)
    {out << *(BDSTrajectoryPoint*)t.GetPoint(i) << G4endl;}
  return out;
}
