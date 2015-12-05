#ifndef BDSOUTPUTROOTEVENTMODEL_H
#define BDSOUTPUTROOTEVENTMODEL_H

#include <vector>
#include <string>

#include "TObject.h"

#ifndef __MAKECINT__
#include "BDSAcceleratorModel.hh"
#endif

class BDSOutputROOTEventModel {
protected :
  std::vector<std::string> name;
  std::vector<float>       length;
  
public : 
  BDSOutputROOTEventModel();
  virtual ~BDSOutputROOTEventModel();
#ifndef __MAKECINT__   
  void Fill();
#endif
  ClassDef(BDSOutputROOTEventModel,1);
};

#endif
