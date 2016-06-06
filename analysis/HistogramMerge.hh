#ifndef HISTOGRAMMERGE_H
#define HISTOGRAMMERGE_H

#include <vector>

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"

#include "BDSOutputROOTEventHistograms.hh"

/**
 * @brief Merge a set of histgrams.
 * 
 * @author Stewart Boogert
 */

class HistogramMerge
{
public:
  /// Default constructor with no action.
  HistogramMerge();

  /// Construct w.r.t. a set of event histograms. Each histogram
  /// is cloned in preparation for accumulation.
  HistogramMerge(BDSOutputROOTEventHistograms *h);
  
  virtual ~HistogramMerge();

  /// Loop over histograms used in non-default constructor and accumulate
  /// those from the input set of histograms.
  void Add(BDSOutputROOTEventHistograms *h);

  /// Loop over (1D only just now) histograms and calculate mean and std of
  /// each bin.
  void Terminate();

  /// Write the resultant histograms to an output file - ignores the argument for now.
  void Write(TFile *outputFile);

private:
  std::vector<TH1D*> histograms1D;
  std::vector<int> histograms1DN;
  std::vector<TH1D*> histograms1DError;
  std::vector<TH2D*> histograms2D;
  std::vector<int> histograms2DN;
  std::vector<TH2D*> histograms2DError;

  ClassDef(HistogramMerge,1);
};

#endif
