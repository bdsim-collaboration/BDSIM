#ifndef ANALYSISOPTIONS_H
#define ANALYSISOPTIONS_H

#include "TROOT.h"

#include "BDSOutputROOTEventOptions.hh"

#include "RebdsimTypes.hh"

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
  void SetBranchAddress(TTree* t,
			bool                      allBranchesOn    = true,
			const RBDS::VectorString* branchesToTurnOn = nullptr);

  /// @{ Member that ROOT can map file data to locally.
  BDSOutputROOTEventOptions* options;
  /// @}

private:
  bool debug;
  
  ClassDef(Options,1);
};

#endif
