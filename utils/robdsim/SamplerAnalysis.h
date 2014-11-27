#ifndef __SamplerAnalysis_h
#define __SamplerAnalysis_h

#include "Sampler.h"

class SamplerAnalysis {
 public : 
  SamplerAnalysis(Sampler *sIn) { s=sIn;} 
  ~SamplerAnalysis() {};
  
  void ProcessEntry();
 private: 
  Sampler *s;
  
};

#endif
