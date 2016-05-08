#ifndef __HISTOGRAMMERGE_H
#define __HISTOGRAMMERGE_H

#include <vector>

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"

#include "BDSOutputROOTEventHistograms.hh"

class HistogramMerge
{
public:
  HistogramMerge();
  HistogramMerge(BDSOutputROOTEventHistograms *h);
  virtual ~HistogramMerge();

  void Add(BDSOutputROOTEventHistograms *h);
  void Terminate();
  void Write(TFile *outputFile);

private:
  std::vector<TH1D*> histograms1D;
  std::vector<TH1D*> histograms1DN;
  std::vector<TH1D*> histograms1DError;
  std::vector<TH2D*> histograms2D;
  std::vector<TH2D*> histograms2DN;
  std::vector<TH2D*> histograms2DError;

  ClassDef(HistogramMerge,1);
};

#endif
