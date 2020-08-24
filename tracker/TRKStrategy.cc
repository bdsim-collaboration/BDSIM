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

TRKStrategy::TRKStrategy(int trackingStepsIn):
  trackingSteps(trackingStepsIn)
{;}

TRKStrategy::~TRKStrategy()
{;}

/*
void TRKStrategy::Track(TRKElement* el, TRKBunch* bunch) {
  Track((TRKDrift*)el,bunch);
}
*/

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
    vector3 newPos(particle.X()-offsetX,particle.Y()-offsetY,particle.Z());
    // add offset (negatively as bunch is displaced, not element)
    particle.SetPosMom(newPos, particle.Mom());
    // rotation
    // TODO, only tilt for new (counterclockwise)
    double sinphi, cosphi;
    //    sincos(el->GetPhi(), &sinphi, &cosphi);
    sinphi = std::sin(el->GetPhi());
    cosphi = std::cos(el->GetPhi());
    double x  = particle.X();
    double y  = particle.Y();
    double xp = particle.Xp();
    double yp = particle.Yp();

    double newx  = cosphi*x  - sinphi*y;
    double newy  = sinphi*x  + cosphi*y;
    double newxp = cosphi*xp - sinphi*yp;
    double newyp = sinphi*xp + cosphi*xp;
    newPos = vector3(newx,newy,particle.Z());
    vector3 newMom(newxp,newyp,particle.Zp());
    particle.SetPosMom(newPos, newMom);

}
