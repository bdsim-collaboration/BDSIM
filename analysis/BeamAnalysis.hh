#ifndef BEAMANALYSIS_H
#define BEAMANALYSIS_H

#include "TROOT.h"

#include "Analysis.hh"

class Beam;
class TChain;

/**
 * @brief Analysis of the options tree.
 * 
 * @author Stuart Walker.
 */

class BeamAnalysis : public Analysis
{
public:
  BeamAnalysis();
  BeamAnalysis(Beam* optionsIn, TChain* chain, bool debug = false);
  virtual ~BeamAnalysis();

  virtual void Process(){};
  virtual void Terminate(){};

protected:
  Beam* beam;
  ClassDef(BeamAnalysis,1);
};

#endif
