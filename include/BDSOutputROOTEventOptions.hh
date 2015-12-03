#ifndef BDSOUTPUTROOTEVENTOPTIONS_H
#define BDSOUTPUTROOTEVENTOPTIONS_H

#include <vector>
#include <string>

#include "TObject.h"

#ifndef __MAKECINT__
#include "BDSGlobalConstants.hh"
#endif

class BDSOutputROOTEventOptions : public TObject {
protected :
  std::vector<std::string> key;
  std::vector<float>       value;
  
public : 
  BDSOutputROOTEventOptions();
  ~BDSOutputROOTEventOptions();
#ifndef __MAKECINT__   
  void Fill();
#endif
  ClassDef(BDSOutputROOTEventOptions,1);
};

#endif
