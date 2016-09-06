#ifndef BDSTRAJECTORY_H
#define BDSTRAJECTORY_H

#include "BDSTrajectoryPoint.hh"
#include "G4Trajectory.hh"

#include <ostream>
#include <vector>

class G4Step;
class G4Track;
class G4TrajectoryContainer;
class G4VTrajectoryPoint;

typedef std::vector<BDSTrajectoryPoint*>  BDSTrajectoryPointsContainer;

/**
 * @brief Trajectory information from track including last scatter etc.
 * 
 * BDSTrajectory stores BDSTrajectoryPoints
 *
 * @author S. Boogert
 */

class BDSTrajectory; // forward declaration so namespaced method can be at top

namespace BDS
{
  /// Search the trajectory container for the primary trajectory.
  BDSTrajectory* GetPrimaryTrajectory(G4TrajectoryContainer* trajCon);
}

class BDSTrajectory: public G4Trajectory
{
public:
  BDSTrajectory(const G4Track* aTrack);
  /// copy constructor is not needed
  BDSTrajectory(BDSTrajectory &) = delete;
  virtual ~BDSTrajectory();

  inline void* operator new(size_t);
  inline void operator delete(void*);
  inline int operator == (const BDSTrajectory& right) const
  {return (this==&right);}

  /// Append a step point to this trajectory. This is required for the trajectory
  /// points to show up in the visualisation correctly.
  virtual void AppendStep(const G4Step* aStep);

  /// Merge another trajectory into this one.
  virtual void MergeTrajectory(G4VTrajectory* secondTrajectory);

  G4VTrajectoryPoint* GetPoint(G4int i) const {return fpBDSPointsContainer[i];}

  virtual int GetPointEntries()    const {return fpBDSPointsContainer.size();}

  G4int GetCreatorProcessType()    const {return creatorProcessType;}
  G4int GetCreatorProcessSubType() const {return creatorProcessSubType;}

  //  void DrawTrajectory() const { G4VTrajectory::DrawTrajectory(); }

  /// Output stream
  friend std::ostream& operator<< (std::ostream &out, BDSTrajectory const &t);

  /// Find the first point in a trajectory where the post step process isn't fTransportation
  /// AND the post step process isn't fGeneral in combination with the post step process subtype
  /// isn't step_limiter.
  static BDSTrajectoryPoint* FirstInteraction(BDSTrajectory* trajectory);
  static BDSTrajectoryPoint* LastInteraction(BDSTrajectory*  trajectory);

protected:
  G4int          creatorProcessType;
  G4int          creatorProcessSubType;
  G4double       weight;

  /// Container of all points. This is really a vector so all memory is dynamically
  /// allocated and there's no need to make this dynamically allocated itself a la
  /// all Geant4 examples.
  BDSTrajectoryPointsContainer fpBDSPointsContainer;

private:
  BDSTrajectory();
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
