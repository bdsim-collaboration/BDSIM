#ifndef BDSTRAJECTORY_H
#define BDSTRAJECTORY_H

#include "G4RichTrajectory.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"

#include <map>
#include <ostream>

/**
 * @brief Trajectory information from track including last scatter etc.
 * 
 * @author L. Deacon.
 */

class BDSTrajectory: public G4RichTrajectory
{
public:
  BDSTrajectory(const G4Track* aTrack);
  BDSTrajectory(BDSTrajectory &);
  virtual ~BDSTrajectory(){}

  inline void* operator new(size_t);
  inline void operator delete(void*);
  inline int operator == (const BDSTrajectory& right) const
  {return (this==&right);}

  /// Append a step point to this trajectory. This is required for the trajectory
  /// points to show up in the visualisation correctly.
  virtual void AppendStep(const G4Step* aStep);

  /// Merge another trajectory into this one.
  virtual void MergeTrajectory(G4VTrajectory* secondTrajectory);

  /// @{ Acessor
  inline G4ThreeVector GetPositionOfLastScatter(G4Track* aTrack) const;
  inline G4ThreeVector GetMomDirAtLastScatter(G4Track* aTrack)   const;
  inline G4double      GetTimeAtLastScatter(G4Track* aTrack)     const;
  inline G4double      GetTimeAtVertex(G4Track* aTrack)          const;
  inline G4double      GetEnergyAtLastScatter(G4Track* aTrack)   const;
  /// @}
  
  /// Output stream
  friend std::ostream& operator<< (std::ostream &out, BDSTrajectory const &t);
  
private:
  /// Private trajectory to force use of supplied one.
  BDSTrajectory();
  
  G4int creatorProcessType;
  G4int creatorProcessSubType;
  
  std::vector<G4int> stepProcessType;
  std::vector<G4int> stepProcessSubType;
  
  std::map<G4int,G4ThreeVector> positionOfLastScatter;
  std::map<G4int,G4ThreeVector> momDirAtLastScatter;
  std::map<G4int,G4double>      energyAtLastScatter;
  std::map<G4int,G4double>      timeAtLastScatter;
  std::map<G4int,G4double>      timeAtVertex;
};

extern G4Allocator<BDSTrajectory> bdsTrajectoryAllocator;

inline void* BDSTrajectory::operator new(size_t)
{
  void* aTrajectory;
  aTrajectory = (void*)bdsTrajectoryAllocator.MallocSingle();
  return aTrajectory;
}

inline void BDSTrajectory::operator delete(void* aTrajectory)
{bdsTrajectoryAllocator.FreeSingle((BDSTrajectory*)aTrajectory);}

inline G4ThreeVector BDSTrajectory::GetPositionOfLastScatter(G4Track* aTrack) const
{return positionOfLastScatter.at(aTrack->GetTrackID());}

inline G4ThreeVector BDSTrajectory::GetMomDirAtLastScatter(G4Track* aTrack) const
{return momDirAtLastScatter.at(aTrack->GetTrackID());}

inline G4double BDSTrajectory::GetEnergyAtLastScatter(G4Track* aTrack) const
{return energyAtLastScatter.at(aTrack->GetTrackID());}

inline G4double BDSTrajectory::GetTimeAtLastScatter(G4Track* aTrack) const
{return timeAtLastScatter.at(aTrack->GetTrackID());}

inline G4double BDSTrajectory::GetTimeAtVertex(G4Track* aTrack) const
{return timeAtVertex.at(aTrack->GetTrackID());}

#endif
