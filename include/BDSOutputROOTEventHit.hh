#ifndef BDSOUTPUTROOTEVENTHIT_H
#define BDSOUTPUTROOTEVENTHIT_H

#include "BDSOutputROOTEventLoss.hh"

#ifndef __ROOTBUILD__
#include "BDSEnergyCounterHit.hh"
#include "BDSTunnelHit.hh"
#endif


class BDSOutputROOTEventHit : public BDSOutputROOTEventLoss {

public:
  /* local coordinates */
  std::vector<float> x;
  std::vector<float> y;
  std::vector<float> z;

  /* Global coordinates */
  std::vector<float> X;
  std::vector<float> Y;
  std::vector<float> Z;

  BDSOutputROOTEventHit();
  virtual ~BDSOutputROOTEventHit();
#ifndef __ROOTBUILD__
  virtual void Fill(BDSEnergyCounterHit *hit);
  virtual void Fill(BDSTunnelHit *hit);
#endif
  virtual void Flush();
  
  ClassDef(BDSOutputROOTEventHit,1);
};

#endif
