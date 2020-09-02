#ifndef TRKOUTPUTSAMPLERS_H
#define TRKOUTPUTSAMPLERS_H

#include <vector>

#include "TRKParticle.hh"
#include "TRKSamplerData.hh"

class TRKOutputSamplers {
public:
  TRKOutputSamplers() = default;
  TRKOutputSamplers(int nsamplers) : samplers(nsamplers) {}
  void RecordParticle(int samplerIndex, TRKParticle p, int turn,
		      double s);
  void AddNSamplers(int n);

private:
  std::vector<TRKSamplerData> samplers;

};

#endif /* TRKOUTPUTSAMPLERS_H */
