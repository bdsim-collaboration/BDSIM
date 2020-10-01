#ifndef OPTICSACCUMULATOR_H
#define OPTICSACCUMULATOR_H

#include <vector>

class TRKParticle;
class SamplerAnalysis;

namespace trk
{
class OpticsAccumulator
{
public:
  void AccumulateParticle(int samplerIndex, const TRKParticle&);
  void AppendAnalysis(double s);
  void Write(std::string);
// private:
public:
  SamplerAnalysis *sam;
  std::vector<SamplerAnalysis*> analyses;
};
}


#endif /* OPTICSACCUMULATOR_H */
