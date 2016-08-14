#ifndef MODEL_H
#define MODEL_H

#include <vector>

#include "TROOT.h"
#include "TChain.h"

#include "BDSOutputROOTEventModel.hh"

class Model
{
public:
  Model();
  virtual ~Model();

  /// Set the branch addresses to address the contents of the file.
  void SetBranchAddress(TChain *c);

  /// @{ Member that ROOT can map file data to locally.
  BDSOutputROOTEventModel* model;
  /// @}

private:
  ClassDef(Model,1);
};

#endif
