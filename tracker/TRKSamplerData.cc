/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
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

void TRKSamplerData::reserve(std::size_t newcap)
{
  particles.reserve(newcap);
}

void TRKSamplerData::SortIfNotSorted()
{
  if (sorted)
    return;
  std::sort(particles.begin(), particles.end(), CompareParticles);
  sorted = true;
}

std::pair<TRKSamplerData::const_iterator, TRKSamplerData::const_iterator>
TRKSamplerData::EventRange(int eventid) {
  SortIfNotSorted();
  struct Comp  // For heterogeneous comparison (as opposed to a lambda)
  {
    bool operator()(TRKSamplerDatum const &lhs, int eventid) {
      return lhs.particle.EventID() < eventid;
    }
    bool operator()(int eventid, TRKSamplerDatum const &rhs) {
      return eventid < rhs.particle.EventID();
    }
  };

  auto range = std::equal_range(particles.begin(), particles.end(),
                                eventid, Comp{});
  return range;
}
