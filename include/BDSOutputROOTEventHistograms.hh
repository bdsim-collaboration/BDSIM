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
  G4int Create1DHistogram(G4String name, G4String title,
                          G4int nbins, G4double xmin, G4double xmax);
  G4int Create1DHistogram(G4String name, G4String title,
                          std::vector<double>& edges);
  G4int Create2DHistogram(G4String name, G4String title,
                          G4int nxbins, G4double xmin, G4double xmax,
                          G4int nybins, G4double ymin, G4double ymax);
  G4int Create2DHistogram(G4String name, G4String title,
                          std::vector<double>& xedges,
                          std::vector<double>& yedges);
  void Fill1DHistogram(G4int histoId, G4double value, G4double weight = 1.0);
  void Fill2DHistogram(G4int histoId, G4double xValue, G4double yValue, G4double weight = 1.0);
#endif

  std::vector<TH1D*>& Get1DHistograms() {return histograms1D;}
  std::vector<TH2D*>& Get2DHistograms() {return histograms2D;}

TH1D* Get1DHistogram(int iHisto) {return histograms1D[iHisto];}
TH2D* Get2DHistogram(int iHisto) {return histograms2D[iHisto];}

private:
  std::vector<TH1D*> histograms1D;
  std::vector<TH2D*> histograms2D;

  ClassDef(BDSOutputROOTEventHistograms,1);
};

#endif