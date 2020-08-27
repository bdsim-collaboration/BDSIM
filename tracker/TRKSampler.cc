/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#include <iostream>
#include <memory>

#include "TRKOutput.hh"
#include "TRKSampler.hh"
#include "TRKStrategy.hh"

TRKSampler::TRKSampler(std::string nameIn,
		       int         indexIn,
		       std::shared_ptr<TRKOutput> outputIn,
		       double sIn):
  TRKElement(nameIn+"_sampler",0,nullptr,nullptr),
  index(indexIn),
  output(std::move(outputIn)),
  s(sIn)
{;}

void TRKSampler::Track(TRKParticle &particle, double /*step*/,
                       TRKStrategy * /*strategy*/) {
  output->FillSamplerHitsTracker(index, particle, s);
}

bool TRKSampler::OutsideAperture(TRKParticle const &) const { return false; }

void TRKSampler::Print(std::ostream &out) const
{
  out << "Sampler: " << name << " ";
}
