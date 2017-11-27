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
#include "TRKApertureCircular.hh"
#include "TRKParticle.hh"
#include "vector3.hh"
#include <iostream>

// radiusIn in meters
TRKApertureCircular::TRKApertureCircular(double radiusIn):radius(radiusIn),radiusSq(radiusIn*radiusIn){;}

TRKApertureCircular::~TRKApertureCircular()
{;}

bool TRKApertureCircular::OutsideAperture(const TRKParticle& particle)
{
  vector3 pos = particle.Pos();

#ifdef TRKDEBUG
  std::cout << "particle pos (um)    " << pos << std::endl;
  std::cout << "aperture rad sq (um) " << radiusSq*1e12 << std::endl;
  std::cout << "pos sq          " << (pos.X()*pos.X() + pos.Y()*pos.Y()) << std::endl;
#endif

  // radiusSq in m^2, convert to um^2
  return radiusSq*1e12 < (pos.X()*pos.X() + pos.Y()*pos.Y());
}

std::ostream& TRKApertureCircular::PrintDetails(std::ostream& out) const
{
  out << "Circular Aperture with radius (m): " << radius
      << " and calculated radius squared (m^2): " << radiusSq << " ";
  return out;
}
