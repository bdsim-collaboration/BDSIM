#include "BDSOutputROOTEventSampler.hh"

class SamplerAnalysis
{
public:
  SamplerAnalysis();
  SamplerAnalysis(BDSOutputROOTEventSampler*);
  ~SamplerAnalysis();
  void Process();
  BDSOutputROOTEventSampler *sampler;
};