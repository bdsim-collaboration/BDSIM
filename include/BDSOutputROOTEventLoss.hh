#ifndef BDSOUTPUTROOTEVENTLOSS_H
#define BDSOUTPUTROOTEVENTLOSS_H

#ifndef __ROOTBUILD__
#include "BDSEnergyCounterHit.hh"
#include "BDSTunnelHit.hh"
#endif

#include "TObject.h"

#include <vector>

class BDSOutputROOTEventLoss : public TObject {

public:
  int                       n = 0;
  std::vector<float>        energy;
  std::vector<float>        S;
  std::vector<float>        weight;
  /* geometry model */
  std::vector<int>          modelID;
  /* process */

  // std::vector<int>          geomFlag;

  BDSOutputROOTEventLoss();
  virtual ~BDSOutputROOTEventLoss();
#ifndef __ROOTBUILD__
  void Fill(BDSEnergyCounterHit *hit);
  void Fill(BDSTunnelHit *hit);
#endif
  virtual void Flush();

  ClassDef(BDSOutputROOTEventLoss,1);
};


#endif
