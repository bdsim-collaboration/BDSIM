#ifndef BDSOUTPUTROOTEVENTSAMPLERMODEL_H
#define BDSOUTPUTROOTEVENTSAMPLERMODEL_H

#include <vector>
#include <string>

#include "TObject.h"
#include "TVector3.h"
#include "TRotation.h"

#ifndef __MAKECINT__
#include "BDSAcceleratorModel.hh"
#endif

#include <string>

class BDSOutputROOTEventSamplerModel
{
public :
  BDSOutputROOTEventSamplerModel();
  virtual ~BDSOutputROOTEventSamplerModel();
  
protected :
  std::vector<std::string> samplerName;
  std::vector<TVector3>    position;
  std::vector<TRotation>   rotation;
  std::vector<double>      sPosition;

#ifndef __MAKECINT__   
  virtual void Fill();
#endif
  ClassDef(BDSOutputROOTEventSamplerModel,1);
};

#endif
