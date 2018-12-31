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
#include "TRKApertureRectangular.hh"

#include <cmath>

#include "TRKParticle.hh"
#include "vector3.hh"

TRKApertureRectangular::TRKApertureRectangular(double xWidthIn, double yWidthIn):
  xWidth(xWidthIn),yWidth(yWidthIn)
{}

TRKApertureRectangular::~TRKApertureRectangular(){}

bool TRKApertureRectangular::OutsideAperture(const TRKParticle& particle)
{
  vector3 pos = particle.Pos();
  return ((std::abs(pos.X()) > xWidth) || (std::abs(pos.Y()) > yWidth));
}

std::ostream& TRKApertureRectangular::PrintDetails(std::ostream& out) const
{
  out << "Rectangular Aperture with x Width (m): " << xWidth
      << "; and y Width (m): " << yWidth << " ";
  return out;
}
