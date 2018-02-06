/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

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
#include "TRKSBend.hh"
#include "TRKStrategy.hh"

TRKSBend::TRKSBend(double        angleIn,
		   std::string   nameIn,
		   double        lengthIn,
		   TRKAperture  *apertureIn,
		   TRKPlacement *placementIn):
  TRKElement(nameIn,lengthIn,apertureIn,placementIn),
  angle(angleIn)
{;}

TRKSBend::~TRKSBend()
{;}

void TRKSBend::Track(TRKBunch* bunch, TRKStrategy* strategy)
{
  strategy->Track(this,bunch);
}

void TRKSBend::Print(std::ostream &out) const
{
  TRKElement::Print(out);
  out << "; Angle: " << angle << "rad";
}
