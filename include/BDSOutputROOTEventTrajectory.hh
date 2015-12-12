#ifndef BDSOUTPUTROOTEVENTTRAJECTORY_H
#define BDSOUTPUTROOTEVENTTRAJECTORY_H

#include "TROOT.h"
#include "TVector3.h"
#include <vector>


class BDSOutputROOTEventTrajectory
{
public:
  BDSOutputROOTEventTrajectory();
  virtual ~BDSOutputROOTEventTrajectory();
  void Flush();

private:

  std::vector<unsigned int> trackID;
  std::vector<std::vector<TVector3>> trajectories;

  ClassDef(BDSOutputROOTEventTrajectory,1);
};


#endif
