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

void TRKStrategy::Track(TRKTiltOffset* el, TRKBunch* bunch)
{
  std::cout << __METHOD_NAME__ << " TiltOffset" << std::endl;

  TRKBunchIter iter = bunch->begin();
  TRKBunchIter end = bunch->end();
  for (;iter!=end;++iter) {
    TRKParticle& part = *iter;
    double offsetX = el->GetOffsetX();
    double offsetY = el->GetOffsetY();
    vector3 newPos(part.X()-offsetX,part.Y()-offsetY,part.Z());
    // add offset (negatively as bunch is displaced, not element)
    part.SetPosMom(newPos, part.Mom());
    // rotation
    // TODO, only tilt for new (counterclockwise)
    double sinphi, cosphi;
    //    sincos(el->GetPhi(), &sinphi, &cosphi);
    sinphi = std::sin(el->GetPhi());
    cosphi = std::cos(el->GetPhi());
    double x  = part.X();
    double y  = part.Y();
    double xp = part.Xp();
    double yp = part.Yp();

    double newx  = cosphi*x  - sinphi*y;
    double newy  = sinphi*x  + cosphi*y;
    double newxp = cosphi*xp - sinphi*yp;
    double newyp = sinphi*xp + cosphi*xp;
    newPos = vector3(newx,newy,part.Z());
    vector3 newMom(newxp,newyp,part.Zp());
    part.SetPosMom(newPos, newMom);
  }
}
