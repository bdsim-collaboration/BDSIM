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
  SamplersData::reference LastSampler() { return samplers.back(); }

  int NSamplers() const { return samplers.size(); }

  SamplersData::reference GetSamplerData(int i) { return samplers.at(i); }

private:
  SamplersData samplers;

};

#endif /* TRKOUTPUTSAMPLERS_H */
