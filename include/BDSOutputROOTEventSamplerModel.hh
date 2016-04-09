#ifndef BDSOUTPUTROOTEVENTSAMPLERMODEL_H
#define BDSOUTPUTROOTEVENTSAMPLERMODEL_H

#include <vector>
#include <string>

#include "TObject.h"
#include "TVector3.h"
#include "TRotation.h"

#include <string>
#include <vector>

/**
 * @brief A record of all samplers in the BDSIM model.
 *
 * Uses ROOT types only for storage in ROOT files.
 * 
 * @author Laurie Nevay
 */


class BDSOutputROOTEventSamplerModel
{
public:
  BDSOutputROOTEventSamplerModel();
  ~BDSOutputROOTEventSamplerModel();
  
protected:
  /// Sampler names
  std::vector<std::string> samplerName;

  /// Sampler positions
  std::vector<TVector3>    position;

  /// Sampler placement rotations - note inverse should be used to transform
  /// sampler hit positions to global coordinates.
  std::vector<TRotation>   rotation;

  /// Curvilinear S position of the sampler (if applicable)
  std::vector<double>      sPosition;

#ifndef __MAKECINT__   
  virtual void Fill();
#endif

  ClassDef(BDSOutputROOTEventSamplerModel,1);
};

#endif
