#ifndef BDSOUTPUTROOTEVENTHIT_H
#define BDSOUTPUTROOTEVENTHIT_H

#include "BDSOutputROOTEventLoss.hh"

class BDSOutputROOTEventHit: public BDSOutputROOTEventLoss
{
public:
  BDSOutputROOTEventHit();
  virtual ~BDSOutputROOTEventHit();
  void Fill();
  void Flush();
  
  ClassDef(BDSOutputROOTEventHit,1);

protected:
  std::vector<float> x;
  std::vector<float> y;
};

#endif
