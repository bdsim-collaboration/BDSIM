#ifndef MODEL_H
#define MODEL_H

#include "TROOT.h"

class BDSOutputROOTEventModel;
class TTree;

class Model
{
public:
  Model();
  virtual ~Model();

  std::vector<std::string> SamplerNames() const;

  /// Set the branch addresses to address the contents of the file.
  void SetBranchAddress(TTree *c);

  /// @{ Member that ROOT can map file data to locally.
  BDSOutputROOTEventModel* model;
  /// @}

private:
  ClassDef(Model,1);
};

#endif
