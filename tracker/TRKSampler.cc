/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2019.

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

#include "TRKSampler.hh"
#include "TRKStrategy.hh"

#include "BDSOutput.hh"

TRKSampler::TRKSampler(std::string nameIn,
		       int         indexIn,
		       BDSOutput*  outputIn):
  TRKElement(nameIn+"_sampler",0,nullptr,nullptr),
  index(indexIn),
  output(outputIn)
{;}

void TRKSampler::Track(TRKBunch* bunch, TRKStrategy* /*strategy*/)
{
  output->FillSamplerHitsTracker(index, bunch);
}

void TRKSampler::Print(std::ostream &out) const
{
  out << "Sampler: " << name << " ";
}