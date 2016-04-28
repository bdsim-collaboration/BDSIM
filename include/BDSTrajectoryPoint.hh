#ifndef BDSTRAJECTORYPOINT_H
#define BDSTRAJECTORYPOINT_H

#include "globals.hh" // geant4 types / globals
#include "G4Allocator.hh"
#include "G4Step.hh"
#include "G4TrajectoryPoint.hh"
#include "G4Track.hh"

#include <ostream>

/**
 * @brief A Point in a trajectory with extra information.
 *
 * @author S. Boogert
 *
 */

class BDSTrajectoryPoint: public G4TrajectoryPoint
{
public:
  BDSTrajectoryPoint();
  BDSTrajectoryPoint(const G4Step* step);
  virtual ~BDSTrajectoryPoint(){;}

  inline void *operator new(size_t);
  inline void operator delete(void *aTrajectoryPoint);
  inline int operator==(const BDSTrajectoryPoint& right) const
  {return (this==&right);};

  /// @{ Accessor
  inline G4int         GetProcessType()      const {return processType;}
  inline G4int         GetProcessSubType()   const {return processSubType;}
  /// @}
  
  /// Output stream
  friend std::ostream& operator<< (std::ostream &out, BDSTrajectoryPoint const &p);
  
private:
  G4int             processType;
  G4int             processSubType;
};

extern G4Allocator<BDSTrajectoryPoint> bdsTrajectoryPointAllocator;

inline void* BDSTrajectoryPoint::operator new(size_t)
{
  void *aTrajectoryPoint;
  aTrajectoryPoint = (void *) bdsTrajectoryPointAllocator.MallocSingle();
  return aTrajectoryPoint;
}

inline void BDSTrajectoryPoint::operator delete(void *aTrajectoryPoint)
{
  bdsTrajectoryPointAllocator.FreeSingle((BDSTrajectoryPoint *) aTrajectoryPoint);
}



#endif
