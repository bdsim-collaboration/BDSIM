#ifndef _BDS_TRAJECTORY_HH__
#define _BDS_TRAJECTORY_HH__

#include "G4Trajectory.hh"
#include "BDSTrajectoryPoint.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include <vector>

class BDSTrajectory: public G4Trajectory{
public:
  BDSTrajectory();
  BDSTrajectory(const G4Track* aTrack);
  BDSTrajectory(BDSTrajectory &);
  virtual ~BDSTrajectory();

  inline void* operator new(size_t);
  inline void operator delete(void*);
  inline int operator == (const BDSTrajectory& right) const
  {return (this==&right);}

  virtual void AppendStep(const G4Step* aStep);
  virtual void MergeTrajectory(const G4VTrajectory* secondTrajectory);

  void printData();
  void printDataOfSteps();
  void printDataOfSteps(G4Step*);
  
  G4ThreeVector GetPositionOfLastScatter(G4Track* aTrack);
  G4ThreeVector GetMomDirAtLastScatter(G4Track* aTrack);
  G4double GetTimeAtLastScatter(G4Track* aTrack);
  G4double GetTimeAtVertex(G4Track* aTrack);
  G4double GetEnergyAtLastScatter(G4Track* aTrack);

private:
  std::map<G4int,G4ThreeVector> _positionOfLastScatter;
  std::map<G4int,G4ThreeVector> _momDirAtLastScatter;
  std::map<G4int,G4double> _energyAtLastScatter;
  std::map<G4int,G4double> _timeAtLastScatter;
  std::map<G4int,G4double> _timeAtVertex;
};

extern G4Allocator<BDSTrajectory> bdsTrajectoryAllocator;

inline void* BDSTrajectory::operator new(size_t){
  void* aTrajectory;
  aTrajectory = (void*)bdsTrajectoryAllocator.MallocSingle();
  return aTrajectory;
}

inline void BDSTrajectory::operator delete(void* aTrajectory){
  bdsTrajectoryAllocator.FreeSingle((BDSTrajectory*)aTrajectory);
}

#endif
