#ifndef BDSOUTPUTROOTEVENTTRAJECTORY_H
#define BDSOUTPUTROOTEVENTTRAJECTORY_H

#include "TROOT.h"
#include "TVector3.h"
#include <vector>

#ifndef __MAKECINT__
#include "BDSTrajectory.hh"
#endif

class BDSOutputROOTEventTrajectory
{
public:
  BDSOutputROOTEventTrajectory();
  virtual ~BDSOutputROOTEventTrajectory();
#ifndef __MAKECINT__
  void Fill(std::vector<BDSTrajectory*> &trajVec);
#endif
  void Flush();

private:

  std::vector<unsigned int> trackID;
  std::vector<std::vector<TVector3>> trajectories;

  ClassDef(BDSOutputROOTEventTrajectory,1);
};


#endif
