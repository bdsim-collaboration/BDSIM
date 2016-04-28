#ifndef BDSOUTPUTROOTEVENTTRAJECTORY_H
#define BDSOUTPUTROOTEVENTTRAJECTORY_H

#include "TROOT.h"
#include "TVector3.h"
#include <vector>

#ifndef __ROOTBUILD__
#include "BDSTrajectory.hh"
#include "BDSEnergyCounterHit.hh"
#endif

class BDSOutputROOTEventTrajectory : public TObject
{
public:
  BDSOutputROOTEventTrajectory();
  virtual ~BDSOutputROOTEventTrajectory();
#ifndef __ROOTBUILD__
  void Fill(std::vector<BDSTrajectory*> &trajVec);
  void Fill(BDSEnergyCounterHitsCollection *phc);
#endif
  void Flush();

  std::vector<unsigned int> trackID;
  std::vector<unsigned int> parentID;
  std::vector<std::vector<int>> processTypes;
  std::vector<std::vector<int>> processSubTypes;
  std::vector<std::vector<TVector3>> trajectories;

  ClassDef(BDSOutputROOTEventTrajectory,1);
};


#endif
