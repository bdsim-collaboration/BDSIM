#ifndef BDSOUTPUTROOTEVENTTRAJECTORY_H
#define BDSOUTPUTROOTEVENTTRAJECTORY_H

#include "TROOT.h"
#include "TVector3.h"
#include <vector>

#ifndef __ROOTBUILD__
#include "BDSTrajectory.hh"
#endif

class BDSOutputROOTEventTrajectory : public TObject
{
public:
  BDSOutputROOTEventTrajectory();
  virtual ~BDSOutputROOTEventTrajectory();
#ifndef __ROOTBUILD__
  void Fill(std::vector<BDSTrajectory*> &trajVec);
#endif
  void Flush();

private:

  std::vector<unsigned int> trackID;
  std::vector<std::vector<TVector3>> trajectories;

  ClassDef(BDSOutputROOTEventTrajectory,1);
};


#endif
