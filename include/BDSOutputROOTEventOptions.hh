#ifndef BDSOUTPUTROOTEVENTOPTIONS_H
#define BDSOUTPUTROOTEVENTOPTIONS_H

#include <vector>
#include <string>

#include "TObject.h"

#ifndef __MAKECINT__
#include "BDSGlobalConstants.hh"
#endif

#include "parser/optionsBase.h"

class BDSOutputROOTEventOptions : public TObject, GMAD::OptionsBase {
protected :
  
public :

  BDSOutputROOTEventOptions();
  BDSOutputROOTEventOptions(GMAD::OptionsBase *);
  virtual ~BDSOutputROOTEventOptions();
#ifndef __MAKECINT__   
  void Fill();
#endif
  ClassDef(BDSOutputROOTEventOptions,1);
};

#endif
