#ifndef _BDS_TRAJECTORY_POINT_HH__
#define _BDS_TRAJECTORY_POINT_HH__

#include "G4RichTrajectoryPoint.hh"
#include "G4Track.hh"

class BDSTrajectoryPoint: public G4RichTrajectoryPoint{
public:
  BDSTrajectoryPoint();
  ~BDSTrajectoryPoint();

  void printRichData();
  
private:
};

#endif
