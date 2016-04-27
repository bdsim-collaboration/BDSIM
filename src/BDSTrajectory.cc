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
  G4RichTrajectory(aTrack)
{
  if(aTrack->GetCreatorProcess() != 0 )
  {
    this->stepProcessType.push_back(aTrack->GetCreatorProcess()->GetProcessType());
    this->stepProcessSubType.push_back(aTrack->GetCreatorProcess()->GetProcessSubType());
    this->creatorProcessType    = aTrack->GetCreatorProcess()->GetProcessType();
    this->creatorProcessSubType = aTrack->GetCreatorProcess()->GetProcessSubType();
  }
  else
  {
    this->stepProcessType.push_back(0);
    this->stepProcessSubType.push_back(0);
    this->creatorProcessType    = 0;
    this->creatorProcessSubType = 0;
  }
  positionOfLastScatter[aTrack->GetTrackID()] = aTrack->GetPosition();
  momDirAtLastScatter[aTrack->GetTrackID()]   = aTrack->GetMomentumDirection();
  energyAtLastScatter[aTrack->GetTrackID()]   = aTrack->GetTotalEnergy();
  timeAtLastScatter[aTrack->GetTrackID()]     = aTrack->GetGlobalTime();
  timeAtVertex[aTrack->GetTrackID()]          = aTrack->GetGlobalTime();
}

void BDSTrajectory::AppendStep(const G4Step* aStep)
{
  if(aStep->GetPreStepPoint()->GetProcessDefinedStep() != 0)
  {
    this->stepProcessType.push_back(aStep->GetPreStepPoint()->GetProcessDefinedStep()->GetProcessType());
    this->stepProcessSubType.push_back(aStep->GetPreStepPoint()->GetProcessDefinedStep()->GetProcessSubType());
  }
  else
  {
    this->stepProcessType.push_back(0);
    this->stepProcessSubType.push_back(0);
  }

  G4RichTrajectory::AppendStep(aStep);
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
  G4cout << __METHOD_NAME__ << G4endl;

  if(!secondTrajectory)
    {return;}

  G4RichTrajectory::MergeTrajectory(secondTrajectory);
  
  BDSTrajectory* seco = (BDSTrajectory*)secondTrajectory;
  for (auto position : seco->positionOfLastScatter)
    {positionOfLastScatter.insert(position);}
  seco->positionOfLastScatter.clear();
}

std::ostream& operator<< (std::ostream& out, BDSTrajectory const& t)
{
  out << t.stepProcessType.size() << " " << t.GetPointEntries() << G4endl;
  for(G4int i = 0; i < t.GetPointEntries(); i++)
   {out << t.stepProcessType[i] << " " << t.stepProcessSubType[i] << " " << *(BDSTrajectoryPoint*)t.GetPoint(i) << G4endl;}
  return out;
}
