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
#include "TRKDipoleFringe.hh"
#include "TRKStrategy.hh"

TRKDipoleFringe::TRKDipoleFringe(
            std::string   nameIn,
		   double        polefaceIn,
		   TRKAperture  *apertureIn,
		   TRKPlacement *placementIn,
		   double        k0In=0.0):
  TRKElement(nameIn, 0.0, apertureIn, placementIn),
  poleface(polefaceIn), k0(k0In)
{;}

void TRKDipoleFringe::Track(TRKParticle& particle, double step, TRKStrategy* strategy)
{
  strategy->Track(this, particle, step);
}
