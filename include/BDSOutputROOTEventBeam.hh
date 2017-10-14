#ifndef BDSOUTPUTROOTEVENTBEAM_H
#define BDSOUTPUTROOTEVENTBEAM_H

#include "TObject.h"

// relative path to suppress ROOT error message (issue #199)
#include "../parser/beamBase.h"

/** 
 * @brief Class to store all beam options for a BDSIM run.
 *
 * @author Laurie Nevay
 */

class BDSOutputROOTEventBeam: public TObject, public GMAD::BeamBase
{
public:
  BDSOutputROOTEventBeam();
  explicit BDSOutputROOTEventBeam(const GMAD::BeamBase* ob);
  virtual ~BDSOutputROOTEventBeam();
  
#ifndef __ROOTBUILD__   
  void Fill();
#endif
  ClassDef(BDSOutputROOTEventBeam,1);
};

#endif
