#ifndef BDSOUTPUTROOTEVENTOPTIONS_H
#define BDSOUTPUTROOTEVENTOPTIONS_H

#include "TObject.h"

// relative path to suppress ROOT error message (issue #199)
#include "../parser/optionsBase.h"

/** 
 * @brief Class to store all options for a BDSIM run.
 *
 * @author Stewart Boogert
 */

class BDSOutputROOTEventOptions: public TObject, public GMAD::OptionsBase
{
public:
  BDSOutputROOTEventOptions();
  explicit BDSOutputROOTEventOptions(const GMAD::OptionsBase* ob);
  virtual ~BDSOutputROOTEventOptions();
  
#ifndef __ROOTBUILD__   
  void Fill();
#endif
  ClassDef(BDSOutputROOTEventOptions,1);
};

#endif
