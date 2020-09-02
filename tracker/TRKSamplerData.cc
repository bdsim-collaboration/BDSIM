#include "TRKSamplerData.hh"


TRKSamplerData::TRKSamplerData(int reserve)
{
  particles.reserve(reserve);
}

void TRKSamplerData::RecordParticle(TRKParticle p, int turn, double s)
{
  TRKSamplerDatum pd = {p, turn, s};
  particles.push_back(pd);
}
