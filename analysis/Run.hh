#ifndef RUN_H
#define RUN_H

#include "TChain.h"

#include "RebdsimTypes.hh"

class BDSOutputROOTEventHistograms;
class BDSOutputROOTEventRunInfo;

/**
 * @brief Class to overlay a run from a ROOT file with.
 * 
 * @author Stewart Boogert
 */

class Run
{
public:
  Run();
  Run(bool debugIn);
  virtual ~Run();

  /// Map the ROOT file to members in this class.
  void SetBranchAddress(TTree* t,
			bool                      allBranchesOn    = true,
			const RBDS::VectorString* branchesToTurnOn = nullptr);
  
  /// @{ Member that ROOT can map file data to locally.
  BDSOutputROOTEventRunInfo*    info;
  BDSOutputROOTEventHistograms* histos;
  /// @}
  
private:  
  bool debug;
  
  ClassDef(Run,1);
};

#endif
