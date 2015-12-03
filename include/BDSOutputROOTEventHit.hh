#ifndef BDSOUTPUTROOTEVENTHIT_H
#define BDSOUTPUTROOTEVENTHIT_H

#include "BDSOutputROOTEventLoss.hh"

class BDSOutputROOTEventHit : public BDSOutputROOTEventLoss {

protected:
  std::vector<float> x;
  std::vector<float> y;
public:
  BDSOutputROOTEventHit();
  ~BDSOutputROOTEventHit();
  void Fill();
  void Flush();
  
  ClassDef(BDSOutputROOTEventHit,1);
};

#endif
