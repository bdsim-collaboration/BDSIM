#ifndef ANALYSIS_OPTIONS_H
#define ANALYSIS_OPTIONS_H

#include "TROOT.h"

#include "BDSOutputROOTEventOptions.hh"

class TTree;

/**
 * @brief Options loader.
 *
 * @author Stuart Walker.
 */

class Options
{
public:
  Options();
  Options(bool debugIn);
  virtual ~Options();

  /// Set the branch addresses to address the contents of the file.
  void SetBranchAddress(TTree *c);

  /// @{ Member that ROOT can map file data to locally.
  BDSOutputROOTEventOptions* options;
  /// @}

private:
  bool debug;
  
  ClassDef(Options,1);
};

#endif
