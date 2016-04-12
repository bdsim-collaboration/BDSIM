#ifndef BDSOUTPUTROOTEVENTHISTOGRAMS_H
#define BDSOUTPUTROOTEVENTHISTOGRAMS_H

#include <vector>

#ifndef __ROOTBUILD__
#include "globals.hh"
#endif

#include "TROOT.h"
#include "TH1D.h"
#include "TH2D.h"

class BDSOutputROOTEventHistograms
{
public:
  BDSOutputROOTEventHistograms();
  virtual ~BDSOutputROOTEventHistograms();

#ifndef __ROOTBUILD__
  void Fill1DHistogram(G4int histoId, G4double value, G4double weight = 1.0);
  void Fill2DHistogram(G4int histoId, G4double xValue, G4double yValue, G4double weight = 1.0);
#endif

private:
  std::vector<TH1D*> histograms1D;
  std::vector<TH2D*> histograms2D;

  ClassDef(BDSOutputROOTEventHistograms,1);
};

#endif