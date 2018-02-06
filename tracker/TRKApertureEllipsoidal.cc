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
#include "TRKAperture.hh"
#include "TRKApertureEllipsoidal.hh"

TRKApertureEllipsoidal::TRKApertureEllipsoidal(double xWidthIn, double yWidthIn):
  xWidth(xWidthIn),yWidth(yWidthIn),xWidthSq(xWidthIn*xWidthIn),yWidthSq(yWidthIn*yWidthIn)
{
  xyWidthSqProd = xWidthSq * yWidthSq;
}

TRKApertureEllipsoidal::~TRKApertureEllipsoidal(){}

bool TRKApertureEllipsoidal::OutsideAperture(const TRKParticle& particle)
{
  vector3 pos = particle.Pos();
  return (yWidthSq*pos.X()*pos.X() + xWidthSq*pos.Y()*pos.Y()) > xyWidthSqProd;
}

std::ostream& TRKApertureEllipsoidal::PrintDetails(std::ostream& out) const
{
  out << "Ellipsoidal Aperture with x Width (m): " << xWidth
      << "; and y Width (m): " << yWidth
      << "; calculated parameters xSq, ySq, xSq*ySq are (m2,m2,m4) "
      << xWidthSq << " " << yWidthSq << " " << xyWidthSqProd << " ";
  return out;
}
