#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "TChain.h"
#include "TH1D.h"
#include "TH2D.h"

#include "HistogramMerge.hh"

class Analysis
{
public:

  Analysis(std::string treeNameIn,
	   TChain*     chainIn,
	   bool        debugIn = false);
  virtual ~Analysis();

  /// Method which calls all other methods.
  virtual void Execute();
  virtual void Process() = 0;
  /// Process histogram definitions from configuration instance.
  virtual void SimpleHistograms();
  virtual void Terminate();
  virtual void Write(TFile *f);  ///< Write rebdsim histogram

protected:
  std::string treeName;
  TChain*     chain;
  std::vector<std::string>    histogramNames; ///< Rebdsim generated histogram names
  std::map<std::string, TH1*> histograms1D;   ///< Rebdsim 1d histogram
  std::map<std::string, TH2*> histograms2D;   ///< Rebdsim 2d histograms
  HistogramMerge*             histoSum;       ///< Bdsim histograms
  bool                        debug;

private:
  Analysis();
  /// Create an individual histogram based on a string selection etc.
  void FillHistogram(std::string treeName, std::string histoName,
		     std::string nbins,    std::string binning,
		     std::string plots,    std::string selection);

};

#endif
