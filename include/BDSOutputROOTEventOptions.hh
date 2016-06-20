#ifndef BDSOUTPUTROOTEVENTOPTIONS_H
#define BDSOUTPUTROOTEVENTOPTIONS_H

#include <vector>
#include <string>

#include "TObject.h"

#ifndef __ROOTBUILD__
#include "BDSGlobalConstants.hh"
#endif

#include "parser/optionsBase.h"

/** 
 * @brief Class to store all options for a BDSIM run.
 *
 * @author Stewart Boogert
 */

class BDSOutputROOTEventOptions: public TObject, GMAD::OptionsBase
{
public:
  BDSOutputROOTEventOptions();
  BDSOutputROOTEventOptions(const GMAD::OptionsBase* ob);
  virtual ~BDSOutputROOTEventOptions();
  
#ifndef __ROOTBUILD__   
  void Fill();
#endif
  ClassDef(BDSOutputROOTEventOptions,1);
};

#endif
