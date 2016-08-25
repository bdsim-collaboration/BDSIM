#ifndef EVENTANALYSIS_H
#define EVENTANALYSIS_H

#include <vector>
#include <map>

#include "TChain.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"

#include "Event.hh"
#include "Analysis.hh"
#include "SamplerAnalysis.hh"

/**
 * @brief Event level analysis.
 *
 * @author Stewart Boogert
 */

class EventAnalysis : public Analysis
{
public:
  EventAnalysis();
  EventAnalysis(Event* eventIn, TChain* chainIn);
  virtual ~EventAnalysis();

  virtual void Process();

  /// Terminate each individual sampler analysis and append optical functions.
  virtual void Terminate();

  /// Write analysis including optical functions to an output file.
  virtual void Write(TFile *outputFileName);

protected:
  Event  *event;
  std::vector<SamplerAnalysis*> samplerAnalyses;
  std::vector<std::vector<std::vector<double>>> opticalFunctions; ///< optical functions from all samplers
  ClassDef(EventAnalysis,1);

private:
  /// Initialise each sampler analysis object in samplerAnalysis.
  void Initialise();
  void ProcessSamplers();
};

#endif
