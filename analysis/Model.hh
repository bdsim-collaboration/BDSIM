#ifndef MODEL_H
#define MODEL_H

#include "TROOT.h"

class BDSOutputROOTEventModel;
class TTree;

/** 
 * @brief Model loader.
 *
 * @author Stuart Walker.
 */

class Model
{
public:
  Model();
  virtual ~Model();

  /// Access all the unique sampler names from the model.
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
