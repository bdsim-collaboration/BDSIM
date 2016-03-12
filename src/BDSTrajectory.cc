#include "BDSDebug.hh"
#include "BDSTrajectory.hh"
#include "BDSTrajectoryPoint.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Step.hh"
#include "G4Track.hh"

#include <map>

G4Allocator<BDSTrajectory> bdsTrajectoryAllocator;

BDSTrajectory::BDSTrajectory()
{;}

BDSTrajectory::BDSTrajectory(const G4Track* aTrack):
  G4Trajectory(aTrack)
{
  _positionOfLastScatter[aTrack->GetTrackID()] = aTrack->GetPosition();
  _momDirAtLastScatter[aTrack->GetTrackID()]   = aTrack->GetMomentumDirection();
  _energyAtLastScatter[aTrack->GetTrackID()]   = aTrack->GetTotalEnergy();
  _timeAtLastScatter[aTrack->GetTrackID()]     = aTrack->GetGlobalTime();
  _timeAtVertex[aTrack->GetTrackID()]          = aTrack->GetGlobalTime();
}

BDSTrajectory::~BDSTrajectory()
{;}

void BDSTrajectory::AppendStep(const G4Step* aStep)
{
  G4Track*            aTrack = aStep->GetTrack();
  BDSTrajectoryPoint* tempTP = new BDSTrajectoryPoint(aTrack);
  if(tempTP->isScatteringProcess())
    {
      _positionOfLastScatter[aTrack->GetTrackID()]=aTrack->GetPosition();
      _momDirAtLastScatter[aTrack->GetTrackID()]=aTrack->GetMomentumDirection();
      _energyAtLastScatter[aTrack->GetTrackID()]=aTrack->GetTotalEnergy();
      _timeAtLastScatter[aTrack->GetTrackID()]=aTrack->GetGlobalTime();
    }
  delete tempTP;
}

void BDSTrajectory::MergeTrajectory(G4VTrajectory* secondTrajectory)
{
  if(!secondTrajectory)
    {return;}

  BDSTrajectory* seco = (BDSTrajectory*)secondTrajectory;
  for (auto position : seco->_positionOfLastScatter)
    {_positionOfLastScatter.insert(position);}
  seco->_positionOfLastScatter.clear();
}

void BDSTrajectory::printData()
{;}

void BDSTrajectory::printDataOfSteps()
{
  BDSTrajectoryPoint* tj;
  for(G4int i = 1; i<GetPointEntries(); i++)
    {
      tj = (BDSTrajectoryPoint*)GetPoint(i);
      G4cout << __METHOD_NAME__ << "Data for trajectory point: " << i << G4endl;
      tj->printData();
    }
}

void BDSTrajectory::printDataOfSteps(G4Step* aStep)
{
  G4int trackID = aStep->GetTrack()->GetTrackID();
  BDSTrajectoryPoint* tj;
  for(G4int i = 0; i<GetPointEntries(); i++)
    {
      tj = (BDSTrajectoryPoint*)GetPoint(i);
      if(tj->GetTrackID() == trackID)
	{
	  G4cout << "BDSTrajectory: Data for trajectory point : " << i << G4endl;
	  tj->printData();
	}
    }
}

G4ThreeVector BDSTrajectory::GetPositionOfLastScatter(G4Track* aTrack)
{return _positionOfLastScatter[aTrack->GetTrackID()];}

G4ThreeVector BDSTrajectory::GetMomDirAtLastScatter(G4Track* aTrack)
{return _momDirAtLastScatter[aTrack->GetTrackID()];}

G4double BDSTrajectory::GetEnergyAtLastScatter(G4Track* aTrack)
{return _energyAtLastScatter[aTrack->GetTrackID()];}

G4double BDSTrajectory::GetTimeAtLastScatter(G4Track* aTrack)
{return _timeAtLastScatter[aTrack->GetTrackID()];}

G4double BDSTrajectory::GetTimeAtVertex(G4Track* aTrack)
{return _timeAtVertex[aTrack->GetTrackID()];}

