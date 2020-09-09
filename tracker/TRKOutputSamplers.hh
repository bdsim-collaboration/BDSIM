#ifndef TRKOUTPUTSAMPLERS_H
#define TRKOUTPUTSAMPLERS_H

#include <vector>
#include <string>

#include "TRKParticle.hh"
#include "TRKSamplerData.hh"

class TRKOutputSamplers {
private:
  using SamplersData = std::vector<TRKSamplerData>;

public:
  using size_type = SamplersData::size_type;

  TRKOutputSamplers() = default;
  TRKOutputSamplers(int nsamplers) : samplers(nsamplers) {}
  void RecordParticle(int samplerIndex, TRKParticle p, int turn,
		      double s);
  void PushBackSampler(const std::string& name);

  using iterator = SamplersData::iterator;
  using const_iterator = SamplersData::const_iterator;
  iterator begin() { return samplers.begin(); }
  iterator end() { return samplers.end(); }
  const_iterator begin() const { return samplers.begin(); }
  const_iterator end() const { return samplers.end(); }
  const_iterator cbegin() const { return samplers.cbegin(); }
  const_iterator cend() const { return samplers.cend(); }

  inline int NSamplers() const { return samplers.size(); }

  inline TRKSamplerData GetSamplerData(int i) { return samplers.at(i); }

private:
  SamplersData samplers;

};

#endif /* TRKOUTPUTSAMPLERS_H */
