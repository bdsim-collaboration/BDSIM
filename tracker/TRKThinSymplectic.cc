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
#include "TRKThinSymplectic.hh"

#include "TRK.hh"
#include "TRKBunch.hh"
//#include "TRKDrift.hh"
#include "TRKParticle.hh"

TRKThinSymplectic::TRKThinSymplectic(int trackingStepsIn):
  TRKThin(trackingStepsIn)
{;}

TRKThinSymplectic::~TRKThinSymplectic()
{;}

void TRKThinSymplectic::Track(TRKDrift* el, TRKBunch* bunch)
{ 

  const double h = el->GetLength()/trackingSteps;

  TRKBunch::iterator iter = bunch->begin();
  TRKBunch::iterator end = bunch->end();
  
  for (;iter!=end;++iter) {
    TRKParticle& part = *iter;
    
    for (int i=0; i<trackingSteps; i++) {

      // double x0 = part.X();
      // double y0 = part.Y();
      // double z0 = part.Z();
      double xp = part.Xp();
      double yp = part.Yp();
      // double zp = part.Zp();

      // TODO, E0, P0 beta0 get from BDSParticle and GConstants
      double beta0=0,E0=0,P0=0, momentum0=0,E=0;
      /// from Sixtrack Physics Manual 3.1 Drift
      double delta = (part.Mom().mag() - momentum0) / momentum0;

      double pt = (E-E0)/P0/TRK::SOL;

      double pz = std::sqrt((1+delta)*(1+delta)-xp*xp-yp*yp);
      double betaz=pz/(1/beta0+pt);

      vector3 dv(xp/pz*h,yp/pz*h,(1-beta0/betaz)*h);
      part.SetPosMom(dv+part.Pos(),part.Mom());
    }
  }
}
