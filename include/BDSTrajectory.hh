#ifndef BDSTRAJECTORY_H
#define BDSTRAJECTORY_H

#include "G4Trajectory.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4TrajectoryContainer.hh"

#include "BDSTrajectoryPoint.hh"
#include "BDSDebug.hh"

#include <map>
#include <ostream>

/**
 * @brief Trajectory information from track including last scatter etc.
 * 
 * BDSTrajectory stores BDSTrajectoryPoints
 *
 * @author S. Boogert
 */

typedef std::vector<BDSTrajectoryPoint*>  BDSTrajectoryPointsContainer;


class BDSTrajectory: public G4Trajectory
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

  G4VTrajectoryPoint* GetPoint(G4int i) const
  { return (*fpBDSPointsContainer)[i];  }

  virtual int GetPointEntries() const { return fpBDSPointsContainer->size(); }

  G4int GetCreatorProcessType()    const {return creatorProcessType;}
  G4int GetCreatorProcessSubType() const {return creatorProcessSubType;}

  //  void DrawTrajectory() const { G4VTrajectory::DrawTrajectory(); }

  /// Output stream
  friend std::ostream& operator<< (std::ostream &out, BDSTrajectory const &t);

  static BDSTrajectoryPoint* FirstLoss(G4TrajectoryContainer *trajCont);
  static BDSTrajectoryPoint* LastLoss(G4TrajectoryContainer *trajCont);

private:
  /// Private trajectory to force use of supplied one.
  BDSTrajectory();

protected:
  G4int          creatorProcessType;
  G4int          creatorProcessSubType;
  G4int          particleID;
  G4double       initKineticEnergy;
  G4ThreeVector  initMomentum;

  BDSTrajectoryPointsContainer *fpBDSPointsContainer;
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


#endif
