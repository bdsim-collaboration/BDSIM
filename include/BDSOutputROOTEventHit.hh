#ifndef BDSOUTPUTROOTEVENTHIT_H
#define BDSOUTPUTROOTEVENTHIT_H

#include "BDSOutputROOTEventLoss.hh"

#ifndef __MAKECINT__
#include "BDSEnergyCounterHit.hh"
#endif


class BDSOutputROOTEventHit : public BDSOutputROOTEventLoss {

protected:
  std::vector<float> x;
  std::vector<float> y;
public:
  BDSOutputROOTEventHit();
  virtual ~BDSOutputROOTEventHit();
#ifndef __MAKECINT__
  virtual void Fill(BDSEnergyCounterHit *hit);
#endif
  virtual void Flush();
  
  ClassDef(BDSOutputROOTEventHit,1);
};

#endif
