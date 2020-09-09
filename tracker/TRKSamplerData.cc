#include "TRKSamplerData.hh"

#include <iterator>


namespace {
  bool CompareParticles(TRKSamplerData::TRKSamplerDatum const &first,
			TRKSamplerData::TRKSamplerDatum const &second)
  {
    if (first.particle.EventID() == second.particle.EventID()) {
      return first.turn < second.turn;
    }
    return first.particle.EventID() < second.particle.EventID();
  }
}

TRKSamplerData::TRKSamplerData(std::string name)
    : samplerName(name), particles(), sorted(true) {}

void TRKSamplerData::RecordParticle(TRKParticle p, int turn, double s)
{
  TRKSamplerDatum pd = {p, turn, s};
  particles.push_back(pd);
  sorted = false;
}



void TRKSamplerData::SortIfNotSorted()
{
  if (sorted)
    return;
  std::sort(particles.begin(), particles.end(), CompareParticles);
  sorted = true;
}

std::pair<TRKSamplerData::const_iterator, TRKSamplerData::const_iterator>
TRKSamplerData::EventRange(int id)  {
  SortIfNotSorted();
  struct Comp  // For heterogeneous comparison (as opposed to a lambda)
  {
    bool operator()(TRKSamplerDatum const &lhs, int id) {
      return lhs.particle.EventID() == id;
    }
    bool operator()(int id, TRKSamplerDatum const &lhs) {
      return lhs.particle.EventID() == id;
    }
  };

  auto range = std::equal_range(particles.begin(), particles.end(),
                                id, Comp{});
  return range;
}

// std::
