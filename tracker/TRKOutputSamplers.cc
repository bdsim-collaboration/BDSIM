#include "TRKOutputSamplers.hh"
#include "TRKParticle.hh"

void TRKOutputSamplers::RecordParticle(int samplerIndex, TRKParticle p, int turn,
				       double s)
{
  samplers.at(samplerIndex).RecordParticle(p, turn, s);
}

void TRKOutputSamplers::PushBackSampler(const std::string& name)
{
  samplers.push_back(TRKSamplerData(name));
}
