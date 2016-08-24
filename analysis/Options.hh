#ifndef ANALYSIS_OPTIONS_H
#define ANALYSIS_OPTIONS_H

#include "TROOT.h"
#include "TChain.h"

#include "BDSOutputROOTEventOptions.hh"

class Options
{
public:
  Options();
  virtual ~Options();

  /// Set the branch addresses to address the contents of the file.
  void SetBranchAddress(TChain *c);

  /// @{ Member that ROOT can map file data to locally.
  BDSOutputROOTEventOptions* options;
  /// @}

private:
  ClassDef(Options,1);
};

#endif
