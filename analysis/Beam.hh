#ifndef ANALYSISBEAM_H
#define ANALYSISBEAM_H

#include "TROOT.h"

#include "BDSOutputROOTEventBeam.hh"

#include "RebdsimTypes.hh"

class TTree;

/**
 * @brief Options loader.
 *
 * @author Laurie Nevay.
 */

class Beam
{
public:
  Beam();
  Beam(bool debugIn);
  virtual ~Beam();

  /// Set the branch addresses to address the contents of the file.
  void SetBranchAddress(TTree* t,
			bool                      allBranchesOn    = true,
			const RBDS::VectorString* branchesToTurnOn = nullptr);

  /// @{ Member that ROOT can map file data to locally.
  BDSOutputROOTEventBeam* beam;
  /// @}

private:
  bool debug;
  
  ClassDef(Beam,1);
};

#endif
