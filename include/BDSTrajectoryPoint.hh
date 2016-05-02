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
  inline G4int         GetPreProcessType()       const {return preProcessType;}
  inline G4int         GetPreProcessSubType()    const {return preProcessSubType;}
  inline G4int         GetPostProcessType()      const {return postProcessType;}
  inline G4int         GetPostProcessSubType()   const {return postProcessSubType;}
  inline G4double      GetPreWeight()            const {return preWeight;}
  inline G4double      GetPostWeight()           const {return postWeight;}
  inline G4double      GetPreEnergy()            const {return preEnergy;}
  inline G4double      GetPostEnergy()           const {return postEnergy;}
  inline G4double      GetEnergy()               const {return energy;}
  inline G4double      GetPreS()                 const {return preS;}
  inline G4double      GetPostS()                const {return postS;}

  /// @}
  
  /// Output stream
  friend std::ostream& operator<< (std::ostream &out, BDSTrajectoryPoint const &p);
  
private:
  G4int              preProcessType;
  G4int              preProcessSubType;
  G4int             postProcessType;
  G4int             postProcessSubType;

  G4double           preWeight;
  G4double          postWeight;

  G4double           preEnergy;
  G4double          postEnergy;
  G4double          energy;

  G4double           preS;
  G4double          postS;
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
