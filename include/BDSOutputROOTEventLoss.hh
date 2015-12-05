#ifndef BDSOUTPUTROOTEVENTLOSS_H
#define BDSOUTPUTROOTEVENTLOSS_H

#ifndef __MAKECINT__
#include  "BDSEnergyCounterHit.hh"
#endif

#include "TObject.h"

#include <vector>

class BDSOutputROOTEventLoss {

protected: 
  int                n;
  std::vector<float> energy;
  std::vector<float> S;
  std::vector<float> weight;

public:
  BDSOutputROOTEventLoss();
  virtual ~BDSOutputROOTEventLoss();
#ifndef __MAKECINT__
  void Fill(BDSEnergyCounterHit *hit);
#endif
  virtual void Flush();

  ClassDef(BDSOutputROOTEventLoss,1);
};


#endif
