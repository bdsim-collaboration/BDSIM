/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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
#include "TRKDecapole.hh"
#include "TRKStrategy.hh"

TRKDecapole::TRKDecapole(double        strengthIn,
			 std::string   nameIn,
			 double        lengthIn,
			 TRKAperture  *apertureIn,
			 TRKPlacement *placementIn):
  TRKElement(nameIn,lengthIn,apertureIn,placementIn, strengthIn)
{;}

TRKDecapole::~TRKDecapole()
{;}

void TRKDecapole::Track(TRKBunch* bunch, TRKStrategy* strategy)
{
  strategy->Track(this,bunch);
}

void TRKDecapole::Print(std::ostream &out) const
{
  TRKElement::Print(out);
  out << "; Strength: " << strength << "T/m^4";
}
