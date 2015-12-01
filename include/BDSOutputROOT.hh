#ifndef BDSOUTPUTROOT_H
#define BDSOUTPUTROOT_H

#ifdef USE_ROOT

#include "BDSOutputROOTBase.hh"
#include "BDSTrajectory.hh"

#include "TROOT.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TTree.h"

/**
 * @brief Lightweight ROOT output class
 * 
 * This contains the basic ROOT output information.
 * 
 * @author Laurie Nevay <Laurie.Nevay@rhul.ac.uk>
 */

class BDSOutputROOT: public BDSOutputROOTBase
{
public:
  /// Default constructor
  BDSOutputROOT(); 
  virtual ~BDSOutputROOT();

  /// Build sampler TTree
  virtual TTree* BuildSamplerTree(G4String name);

  void FillEvent() {};

};

#endif // USE_ROOT
#endif
