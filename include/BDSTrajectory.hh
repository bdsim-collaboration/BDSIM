#ifndef _BDS_TRAJECTORY_HH__
#define _BDS_TRAJECTORY_HH__

#include "G4RichTrajectory.hh"
#include "G4Track.hh"

class BDSTrajectory: public G4RichTrajectory{
public:
  BDSTrajectory(const G4Track* aTrack);
  ~BDSTrajectory();

  void printRichData();
  
private:
};

#endif
