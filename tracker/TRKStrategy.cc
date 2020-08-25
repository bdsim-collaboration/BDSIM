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
#include "TRKStrategy.hh"

#include "BDSDebug.hh"
#include "BDSOutputBase.hh"
#include "TRKSampler.hh"
#include "TRKTiltOffset.hh"
#include "vector3.hh"
#include "vector6.hh"

extern BDSOutputBase* trkOutput;


void TRKStrategy::SetMomentumAndEnergy(double nominalMomentumIn,
				       double nominalEnergyIn)
{
  nominalMomentum = nominalMomentumIn;
  nominalEnergy   = nominalEnergyIn;
}

void TRKStrategy::Track(TRKTiltOffset* el, TRKParticle& particle)
{
  std::cout << __METHOD_NAME__ << " TiltOffset" << std::endl;

  double offsetX = el->GetOffsetX();
  double offsetY = el->GetOffsetY();
  particle.x -= offsetX;
  particle.y -= offsetY;
  // rotation
  // TODO, only tilt for new (counterclockwise)
  double sinphi, cosphi;
  //    sincos(el->GetPhi(), &sinphi, &cosphi);
  sinphi = std::sin(el->GetPhi());
  cosphi = std::cos(el->GetPhi());
  double x = particle.x;
  double y = particle.y;
  double px = particle.px;
  double py = particle.py;

  double newx = cosphi * x - sinphi * y;
  double newy = sinphi * x + cosphi * y;
  double newpx = cosphi * px - sinphi * py;
  double newpy = sinphi * px + cosphi * px;
  particle.x = newx;
  particle.y = newy;
  particle.px = newpx;
  particle.py = newpy;
}
