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
#ifndef TRKSAMPLERDATA_H
#define TRKSAMPLERDATA_H

#include <cstddef>
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
  using SamplerParticles = std::vector<TRKSamplerDatum>;

  TRKSamplerData() = default;
  TRKSamplerData(std::string name);

  void RecordParticle(TRKParticle p, int turn, double s);

  using iterator = SamplerParticles::iterator;
  using const_iterator = SamplerParticles::const_iterator;
  iterator begin() { return particles.begin(); }
  iterator end() { return particles.end(); }
  const_iterator begin() const { return particles.begin(); }
  const_iterator end() const { return particles.end(); }
  const_iterator cbegin() const { return particles.cbegin(); }
  const_iterator cend() const { return particles.cend(); }

  std::pair<const_iterator, const_iterator> EventRange(int eventNumber);

  void reserve(std::size_t newcap);

  std::string Name() const { return samplerName; }

private:
  std::string samplerName;
  SamplerParticles particles;
  bool sorted; // dirty bit
  void SortIfNotSorted();


};

#endif /* TRKSAMPLERDATA_H */
