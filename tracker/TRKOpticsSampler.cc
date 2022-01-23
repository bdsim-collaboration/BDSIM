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
#include <string>
#include <memory>

#include "OpticsAccumulator.hh"
#include "EventOutput.hh"
#include "TRKOpticsSampler.hh"
#include "TRKStrategy.hh"

#include "analysis/SamplerAnalysis.hh"

TRKOpticsSampler::TRKOpticsSampler(std::string nameIn,
				   int         indexIn,
				   double sIn,
				   std::shared_ptr<trk::OpticsAccumulator> opticsIn):
  TRKElement(nameIn),
  index(indexIn),
  s(sIn),
  optics(std::move(opticsIn))
{}

void TRKOpticsSampler::Track(TRKParticle &particle, double, TRKStrategy *) {
  optics->AccumulateParticle(index, particle);
}
