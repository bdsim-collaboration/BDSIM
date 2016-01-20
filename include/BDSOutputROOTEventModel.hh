#ifndef BDSOUTPUTROOTEVENTMODEL_H
#define BDSOUTPUTROOTEVENTMODEL_H

#include "TObject.h"

#include <vector>
#include <string>


class BDSOutputROOTEventModel : public TObject {
protected :
  std::vector<std::string> name;
  std::vector<float>       length;
  
public : 
  BDSOutputROOTEventModel();
  ~BDSOutputROOTEventModel();  

  ClassDef(BDSOutputROOTEventModel,1);
};

#endif
