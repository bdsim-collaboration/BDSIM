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
#ifndef TRKOpticsSampler_h
#define TRKOpticsSampler_h

#include <memory>
#include <string>

#include "TRKElement.hh"

class SamplerAnalysis;
class TRKStrategy;
class TRKBunch;


namespace trk
{
  class EventOutput;
  class OpticsAccumulator;
}

/**
 * @brief sampler class - output from tracker
 */

class TRKOpticsSampler : public TRKElement
{
public:
  TRKOpticsSampler() = delete;
  TRKOpticsSampler(std::string nameIn,
		   int         indexIn,
		   double s,
		   std::shared_ptr<trk::OpticsAccumulator>);
  void Track(TRKParticle &particle, double step,
             TRKStrategy *strategy) override;

private:
  int index;
  double s;
  std::shared_ptr<trk::OpticsAccumulator> optics;
};

#endif
