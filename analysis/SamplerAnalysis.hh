#include "BDSOutputROOTEventSampler.hh"

class SamplerAnalysis
{
public:
  SamplerAnalysis();
  SamplerAnalysis(BDSOutputROOTEventSampler*);
  ~SamplerAnalysis();

protected:
  BDSOutputROOTEventSampler *sampler;
};