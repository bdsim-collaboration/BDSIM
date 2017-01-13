#ifndef MODELANALYSIS_H
#define MODELANALYSIS_H

#include "TROOT.h"

#include "Analysis.hh"

class Model;
class TChain;

/**
 * @brief Analysis of the model tree.
 *
 * @author Stuart Walker.
 */

class ModelAnalysis : public Analysis
{
public:
  ModelAnalysis();
  ModelAnalysis(Model* model, TChain* chain, bool debug = false);
  virtual ~ModelAnalysis(){};

  virtual void Process(){};
  virtual void Terminate(){};

protected:
  Model* model;
  ClassDef(ModelAnalysis,1);
};


#endif
