#ifndef BDSTRAJECTORYPOINT_H
#define BDSTRAJECTORYPOINT_H

#include "BDSAuxiliaryNavigator.hh"

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
 */

class BDSTrajectoryPoint: public G4TrajectoryPoint
{
public:
  /// Default constructor.
  BDSTrajectoryPoint();

  /// This constructor is used to start a trajectory from a track. After
  /// This, points may be constructed from steps.
  BDSTrajectoryPoint(const G4Step* step);
  
  /// This constructor is required for the beginning of each track
  /// and produces the initial vertex point.
  BDSTrajectoryPoint(const G4Track* track);

  virtual ~BDSTrajectoryPoint();

  inline void *operator new(size_t);
  inline void operator delete(void *aTrajectoryPoint);
  inline int operator==(const BDSTrajectoryPoint& right) const
  {return (this==&right);};

  /// @{ Accessor
  inline G4int    GetPreProcessType()     const {return preProcessType;}
  inline G4int    GetPreProcessSubType()  const {return preProcessSubType;}
  inline G4int    GetPostProcessType()    const {return postProcessType;}
  inline G4int    GetPostProcessSubType() const {return postProcessSubType;}
  inline G4double GetPreWeight()          const {return preWeight;}
  inline G4double GetPostWeight()         const {return postWeight;}
  inline G4double GetPreEnergy()          const {return preEnergy;}
  inline G4double GetPostEnergy()         const {return postEnergy;}
  inline G4double GetEnergy()             const {return energy;}
  inline G4double GetPreS()               const {return preS;}
  inline G4double GetPostS()              const {return postS;}
  inline G4int    GetBeamLineIndex()      const {return beamlineIndex;}
  inline G4int    GetTurnsTaken()         const {return turnstaken;}
  inline G4ThreeVector GetPrePosLocal()   const {return prePosLocal;}
  inline G4ThreeVector GetPostPosLocal()  const {return postPosLocal;}
  /// @}
  
  /// Output stream
  friend std::ostream& operator<< (std::ostream &out, BDSTrajectoryPoint const &p);
  
private:
  /// Initialisation of variables in separate function to reduce duplication in
  /// multiple constructors.
  void InitialiseVariables();
  
  G4int preProcessType;      ///< Process type of pre-step point
  G4int preProcessSubType;   ///< Process sub type of pre-step point
  G4int postProcessType;     ///< Process type of post step point
  G4int postProcessSubType;  ///< Process sub type of post step point

  G4double preWeight;        ///< Weight associated with pre-step point
  G4double postWeight;       ///< Weight associtaed with post step point
  G4double preEnergy;        ///< Kinetic energy of pre-step point
  G4double postEnergy;       ///< Kinetic energy of post step point
  G4double energy;           ///< Total energy deposited during step
  G4double preS;             ///< Global curvilinear S coordinate of pre-step point
  G4double postS;            ///< Global curvilinear S coordinate of post step point
  G4int    beamlineIndex;    ///< Index to beam line elment
  G4int    turnstaken;       ///< Number of turns taken
  G4ThreeVector prePosLocal; ///< Local coordinates of pre-step point
  G4ThreeVector postPosLocal;///< Local coordinates of post-step point
  
  /// An auxilliary navigator to get curvilinear coordintes. Lots of points, but only
  /// need one navigator so make it static.
  static BDSAuxiliaryNavigator* auxNavigator;

  /// Keep track of how many points are instantiated and delete the static navigator
  /// when the last point is deleted.
  static G4int numberOfPoints;
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
