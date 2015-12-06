#ifndef BDSOUTPUTROOTEVENTPRIMARY_H
#define BDSOUTPUTROOTEVENTPRIMARY_H

#include "TROOT.h"
#include <vector>

class BDSOutputROOTEventPrimary : public TObject {
protected:

public:
  BDSOutputROOTEventPrimary();
  ~BDSOutputROOTEventPrimary();
  
#ifndef __MAKECINT__ 
  void Fill(BDSSamplerHit *hit);
#endif

  /// Clean primary
  void Flush;

  ClassDef(BDSOutputROOTEventPrimary,1);
};


#endif
