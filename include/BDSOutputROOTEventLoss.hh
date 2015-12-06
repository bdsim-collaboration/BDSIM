#ifndef BDSOUTPUTROOTEVENTLOSS_H
#define BDSOUTPUTROOTEVENTLOSS_H

#ifndef __MAKECINT__
#include "BDSEnergyCounterHit.hh"
#include "BDSTunnelHit.hh"
#endif

#include "TObject.h"

#include <vector>

class BDSOutputROOTEventLoss {

protected: 
  int                       n;
  std::vector<float>        energy;
  std::vector<float>        S;
  std::vector<float>        weight;
  std::vector<unsigned int> indexModel;

public:
  BDSOutputROOTEventLoss();
  virtual ~BDSOutputROOTEventLoss();
#ifndef __MAKECINT__
  void Fill(BDSEnergyCounterHit *hit);
  void Fill(BDSTunnelHit *hit);
#endif
  virtual void Flush();

  ClassDef(BDSOutputROOTEventLoss,1);
};


#endif
