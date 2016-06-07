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
  /* turn number */
  std::vector<int>          turn;

  /* local coordinates */
  std::vector<float>        x;
  std::vector<float>        y;
  std::vector<float>        z;

  /* global coordinates */
  std::vector<float>        X;
  std::vector<float>        Y;
  std::vector<float>        Z;
  
  BDSOutputROOTEventLoss();
  BDSOutputROOTEventLoss(bool storeLocal, bool storeGobal);
  virtual ~BDSOutputROOTEventLoss();
#ifndef __ROOTBUILD__
  void Fill(BDSEnergyCounterHit *hit);
  void Fill(BDSTunnelHit *hit);
#endif
  virtual void Flush();

  bool storeLocal  = false;
  bool storeGlobal = false;
  
  ClassDef(BDSOutputROOTEventLoss,1);
};


#endif
