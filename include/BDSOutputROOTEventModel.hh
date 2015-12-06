#ifndef BDSOUTPUTROOTEVENTMODEL_H
#define BDSOUTPUTROOTEVENTMODEL_H

#include <vector>
#include <string>

#include "TObject.h"
#include "TVector3.h"
#include "TRotation.h"

#ifndef __MAKECINT__
#include "BDSAcceleratorModel.hh"
#endif

class BDSOutputROOTEventModel {
protected :
  std::vector<std::string> name;
  std::vector<float>       length;
  std::vector<TVector3>    staPos;
  std::vector<TVector3>    midPos;
  std::vector<TVector3>    endPos;
  std::vector<TRotation>   staRot;
  std::vector<TRotation>   midRot;
  std::vector<TRotation>   endRot;

public :
  BDSOutputROOTEventModel();
  virtual ~BDSOutputROOTEventModel();

#ifndef __MAKECINT__   
  virtual void Fill();
#endif
  ClassDef(BDSOutputROOTEventModel,1);
};

#endif
