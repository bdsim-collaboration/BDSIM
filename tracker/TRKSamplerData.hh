#ifndef TRKSAMPLERDATA_H
#define TRKSAMPLERDATA_H

#include <vector>

#include "TRKParticle.hh"


class TRKSamplerData
{
public:
  struct TRKSamplerDatum {
    TRKParticle particle;
    int turn;
    double s;
  };

  TRKSamplerData() = default;
  TRKSamplerData(int reserve);

  void RecordParticle(TRKParticle p, int turn, double s);

private:
  std::vector<TRKSamplerDatum> particles;
};

#endif /* TRKSAMPLERDATA_H */
