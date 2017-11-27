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
#include "TRKPlacement.hh"

TRKPlacement::TRKPlacement() : x(0.0), y(0.0), z(0.0) 
{
}

TRKPlacement::TRKPlacement(double xIn, double yIn, double zIn) : x(xIn), y(yIn), z(zIn) 
{
}

TRKPlacement::~TRKPlacement() {}

vector3 TRKPlacement::GetGlobal()const
{
  return vector3(x,y,z);

}

std::ostream& operator<< (std::ostream& out, const TRKPlacement& /*placement*/) {
  out << "Placement - TBC ";
  return out;
}
