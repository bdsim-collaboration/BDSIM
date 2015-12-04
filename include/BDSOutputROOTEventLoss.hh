#ifndef BDSOUTPUTROOTEVENTLOSS_H
#define BDSOUTPUTROOTEVENTLOSS_H

#include "TObject.h"

#include <vector>

class BDSOutputROOTEventLoss : public TObject {

protected: 
  int                n;
  std::vector<float> energy;
  std::vector<float> S;
  std::vector<float> weight;

public:
  BDSOutputROOTEventLoss();
  ~BDSOutputROOTEventLoss();
  virtual void Fill();
  void Flush();

  ClassDef(BDSOutputROOTEventLoss,1);
};


#endif
