#include "BDSTrajectory.hh"
#include "BDSTrajectoryPoint.hh"
#include "G4AttDef.hh"
#include <map>

G4Allocator<BDSTrajectory> bdsTrajectoryAllocator;


BDSTrajectory::BDSTrajectory():G4Trajectory(){
}

BDSTrajectory::BDSTrajectory(const G4Track* aTrack):G4Trajectory(aTrack){
  _positionOfLastScatter[aTrack->GetTrackID()]=aTrack->GetPosition();
  _momDirAtLastScatter[aTrack->GetTrackID()]=aTrack->GetMomentumDirection();
  _energyAtLastScatter[aTrack->GetTrackID()]=aTrack->GetTotalEnergy();
  _timeAtLastScatter[aTrack->GetTrackID()]=aTrack->GetGlobalTime();
  _timeAtVertex[aTrack->GetTrackID()]=aTrack->GetGlobalTime();
}

BDSTrajectory::~BDSTrajectory(){
}

void BDSTrajectory::AppendStep(const G4Step* aStep){
  G4Track* aTrack = aStep->GetTrack();
  BDSTrajectoryPoint* tempTP = new BDSTrajectoryPoint(aTrack);
  if(tempTP->isScatteringProcess()){
    _positionOfLastScatter[aTrack->GetTrackID()]=aTrack->GetPosition();
    _momDirAtLastScatter[aTrack->GetTrackID()]=aTrack->GetMomentumDirection();
    _energyAtLastScatter[aTrack->GetTrackID()]=aTrack->GetTotalEnergy();
    _timeAtLastScatter[aTrack->GetTrackID()]=aTrack->GetGlobalTime();
  }
  delete tempTP;
}

void BDSTrajectory::MergeTrajectory(const G4VTrajectory* secondTrajectory){
  if(!secondTrajectory) return;

  BDSTrajectory* seco = (BDSTrajectory*)secondTrajectory;
  for(std::map<G4int, G4ThreeVector>::iterator iter = seco->_positionOfLastScatter.begin();
      iter!=seco->_positionOfLastScatter.end();
      iter++){
    _positionOfLastScatter.insert(*iter);
  }
  seco->_positionOfLastScatter.clear();
}

void BDSTrajectory::printData(){
}

void BDSTrajectory::printDataOfSteps(){
  BDSTrajectoryPoint* tj;
  for(int i = 0; i<GetPointEntries(); i++){
    tj = (BDSTrajectoryPoint*)GetPoint(i);
    if(i>0){
      G4cout << "BDSTrajectory: Data for trajectory point : " << i << G4endl;
      tj->printData();
    }
  }
}

void BDSTrajectory::printDataOfSteps(G4Step* aStep){
  G4int trackID = aStep->GetTrack()->GetTrackID();
  BDSTrajectoryPoint* tj;
  for(int i = 0; i<GetPointEntries(); i++){
    tj = (BDSTrajectoryPoint*)GetPoint(i);
    if(tj->GetTrackID() == trackID){
      G4cout << "BDSTrajectory: Data for trajectory point : " << i << G4endl;
      tj->printData();
    }
  }
}

G4ThreeVector BDSTrajectory::GetPositionOfLastScatter(G4Track* aTrack){
  return _positionOfLastScatter[aTrack->GetTrackID()];
}

G4ThreeVector BDSTrajectory::GetMomDirAtLastScatter(G4Track* aTrack){
  return _momDirAtLastScatter[aTrack->GetTrackID()];
}

G4double BDSTrajectory::GetEnergyAtLastScatter(G4Track* aTrack){
  return _energyAtLastScatter[aTrack->GetTrackID()];
}

G4double BDSTrajectory::GetTimeAtLastScatter(G4Track* aTrack){
  return _timeAtLastScatter[aTrack->GetTrackID()];
}

G4double BDSTrajectory::GetTimeAtVertex(G4Track* aTrack){
  return _timeAtVertex[aTrack->GetTrackID()];
}

