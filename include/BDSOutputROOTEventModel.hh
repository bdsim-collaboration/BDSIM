#ifndef BDSOUTPUTROOTEVENTMODEL_H
#define BDSOUTPUTROOTEVENTMODEL_H

#include <vector>
#include <string>

#include "TObject.h"
#include "TVector3.h"
#include "TRotation.h"

class BDSOutputROOTEventModel: public TObject
{
public:
  std::vector<std::string> samplerNamesUnique;

  std::vector<std::string> componentName;
  std::vector<std::string> placementName;
  std::vector<std::string> componentType;
  std::vector<float>       length;
  std::vector<TVector3>    staPos;
  std::vector<TVector3>    midPos;
  std::vector<TVector3>    endPos;
  std::vector<TRotation>   staRot;
  std::vector<TRotation>   midRot;
  std::vector<TRotation>   endRot;
  std::vector<TVector3>    staRefPos;
  std::vector<TVector3>    midRefPos;
  std::vector<TVector3>    endRefPos;
  std::vector<TRotation>   staRefRot;
  std::vector<TRotation>   midRefRot;
  std::vector<TRotation>   endRefRot;
  std::vector<float>       staS;
  std::vector<float>       midS;
  std::vector<float>       endS;

  BDSOutputROOTEventModel();
  virtual ~BDSOutputROOTEventModel();

#ifndef __ROOTBUILD__   
  virtual void Fill();
#endif


  ClassDef(BDSOutputROOTEventModel,1);
};

#endif
