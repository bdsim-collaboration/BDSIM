#ifndef BDSOUTPUTROOT_H
#define BDSOUTPUTROOT_H

#include "BDSOutputROOTBase.hh"

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

#endif
