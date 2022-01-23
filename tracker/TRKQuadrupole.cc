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
#include "TRKQuadrupole.hh"
#include "TRKStrategy.hh"
#include "TRKTiltOffset.hh"
#include "TRKBunch.hh"

TRKQuadrupole::TRKQuadrupole(std::string   nameIn,
			     double        lengthIn,
			     double        k1in,
			     TRKAperture*  apertureIn,
			     TRKPlacement* placementIn):
  TRKElement(nameIn, lengthIn, apertureIn, placementIn),
  k1(k1in)
{;}

void TRKQuadrupole::Track(TRKParticle& particle, double step, TRKStrategy* strategy)
{

  if (offsetIn && offsetOut)
    {strategy->Track(offsetIn, particle);}

  strategy->Track(this, particle, step);

  if (offsetIn && offsetOut)
    {strategy->Track(offsetOut, particle);}
}
