#ifndef OPTIONSANALYSIS_H
#define OPTIONSANALYSIS_H

#include "TROOT.h"

#include "Analysis.hh"

class Options;
class TChain;

/**
 * @brief Analysis of the options tree.
 * 
 * @author Stuart Walker.
 */

class OptionsAnalysis : public Analysis
{
public:
  OptionsAnalysis();
  OptionsAnalysis(Options* optionsIn, TChain* chain, bool debug = false);
  virtual ~OptionsAnalysis();

  virtual void Process(){};
  virtual void Terminate(){};

protected:
  Options* options;
  ClassDef(OptionsAnalysis,1);
};

#endif
