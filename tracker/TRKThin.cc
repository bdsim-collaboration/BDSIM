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
#include "TRKThin.hh"

#include <cmath>
#include <complex>
#include <cstdlib>
#include <iostream>

#include "BDSDebug.hh"

#include "TRKBunch.hh"
#include "TRKHybrid.hh"
#include "TRKParticle.hh"
//#include "TRKDrift.hh"
#include "TRKDipole.hh"
#include "TRKQuadrupole.hh"
#include "TRKSextupole.hh"
#include "TRKOctupole.hh"

TRKThin::TRKThin(int trackingStepsIn) : 
  TRKStrategy(trackingStepsIn) {
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << "with " << trackingStepsIn 
	    << " tracking steps per element" << std::endl;
#endif
}

TRKThin::~TRKThin() { 
}

void TRKThin::Track(TRKDrift* el, TRKBunch* bunch) { 
  // this drift method can be done in one go, trackingsteps not needed;
  const double h = el->GetLength(); 
  TRKBunch::iterator iter = bunch->begin();
  TRKBunch::iterator end  = bunch->end();

  for (;iter!=end;++iter) {
    TRKParticle& part = *iter;
    
    vector3 unit = part.Mom().unit();
    vector3 dv = unit * h / unit.Z();   
    
    part.SetPosMom(dv+part.Pos(),part.Mom());   
  }
}

void TRKThin::Track(TRKDipole* el, TRKBunch* bunch) { 
  // could use from Sixtrack Physics Manual 3.2.2 Thin Dipole expanded Hamiltonian
  /// simple Dipole for now with half drifts

  // use integrated strength
  const double strength = el->GetIntStrength();
  if (std::abs(strength)<=1e-12) {
    return Track((TRKDrift*)el,bunch);
  }

  // TODO : multiple trackingsteps
  //  const double h = el->GetLength()/trackingSteps;
  // for (int i=0; i<trackingSteps; i++) {
    
  TRKBunch::iterator iter = bunch->begin();
  TRKBunch::iterator end = bunch->end();
  
  // half drift
  Track((TRKDrift*)el,bunch);
  
  for (;iter!=end;++iter) {
    TRKParticle& part = *iter;
    vector3 mom = part.Mom();
    //double dx = mom.X() + strength / part.E(); // LN temporary to get to compile
    double dx = mom.X() + strength / part.P();
    vector3 momnew = vector3(dx,mom.Y(),mom.Z());
    part.SetPosMom(part.Pos(),momnew);
  }
  // half drift
  Track((TRKDrift*)el,bunch);
}

void TRKThin::Track(TRKSBend* el, TRKBunch* bunch) { 
  return Track((TRKDrift*)el,bunch);
}

void TRKThin::Track(TRKRBend* el, TRKBunch* bunch) { 
  return Track((TRKDrift*)el,bunch);
}

void TRKThin::Track(TRKQuadrupole* el, TRKBunch* bunch) { 
  const double strength = el->GetStrength();
  if (std::abs(strength)<=1e-12) {
    return Track((TRKDrift*)el,bunch);
  }

  const double h = el->GetLength()/trackingSteps;

  TRKBunch::iterator iter = bunch->begin();
  TRKBunch::iterator end = bunch->end();
  
  // initialise kick // only needed once
  const double Kn = strength;

  for (;iter!=end;++iter) {
    TRKParticle& part = *iter;
    // paraxial approximation ONLY!!
    if(iter== bunch->begin() && (std::abs(part.Zp())>0.99)&&(std::abs(strength)<1.e-6)) {
      TRKHybrid hybrid(trackingSteps);
      return hybrid.Track(el,bunch);
    }
    
    for (int i=0; i<trackingSteps; i++) {

      double x0 = part.X();
      double y0 = part.Y();
      double z0 = part.Z();
      double xp = part.Xp();
      double yp = part.Yp();
      double zp = part.Zp();

      // calculate particle kick
      std::complex<double> Kick = Kn * std::complex<double>(x0, y0);
      double kickxp = -real(Kick);
      double kickyp = +imag(Kick);

      // apply kick, 2 steps
      double xp_ = xp + kickxp * 0.5;
      double yp_ = yp + kickyp * 0.5;
    
      double vOut[6];

      vOut[0] = x0 + xp_ * h;
      vOut[1] = y0 + yp_ * h;
      vOut[2] = z0 + (0.5e-6 * (xp_*xp_ + yp_*yp_) * h); // to be checked

      // calculate particle kick
      Kick = Kn * std::complex<double>(vOut[0], vOut[1]);
      kickxp = -real(Kick);
      kickyp = +imag(Kick);

      vOut[3] = xp_ + kickxp * 0.5;
      vOut[4] = yp_ + kickyp * 0.5;
      vOut[5] = zp;
      part.SetPosMom(vector6(vOut));
    }
  }
}

void TRKThin::Track(TRKSextupole* el, TRKBunch* bunch) { 
  const double strength = el->GetStrength();
  if (std::abs(strength)<=1e-12) {
    return Track((TRKDrift*)el,bunch);
  }

  const double h = el->GetLength()/trackingSteps;

  TRKBunch::iterator iter = bunch->begin();
  TRKBunch::iterator end = bunch->end();
  
  // initialise kick // only needed once
  double Kn = strength * 0.5;

  for (;iter!=end;++iter) {
    TRKParticle& part = *iter;
    // paraxial approximation ONLY!!
    if(iter== bunch->begin() && (std::abs(part.Zp())>0.99)&&(std::abs(strength)<1.e-6)) {
      TRKHybrid hybrid(trackingSteps);
      return hybrid.Track(el,bunch);
    }
    
    for (int i=0; i<trackingSteps; i++) {

      double x0 = part.X();
      double y0 = part.Y();
      double z0 = part.Z();
      double xp = part.Xp();
      double yp = part.Yp();
      double zp = part.Zp();

      // adapted from PLACET, element_thin_lens

      // calculate particle kick
      std::complex<double> Kick = Kn * std::pow(std::complex<double>(x0, y0),2);
      double kickxp = -real(Kick);
      double kickyp = +imag(Kick);

      // apply kick, 2 steps
      double xp_ = xp + kickxp * 0.5;
      double yp_ = yp + kickyp * 0.5;

      double vOut[6];
      vOut[0] = x0 + xp_ * h;
      vOut[1] = y0 + yp_ * h;
      vOut[2] = z0 + (0.5e-6 * (xp_*xp_ + yp_*yp_) * h); // to be checked

      // calculate particle kick
      Kick = Kn * std::pow(std::complex<double>(vOut[0], vOut[1]),2);
      kickxp = -real(Kick);
      kickyp = +imag(Kick);

      vOut[3] = xp_ + kickxp * 0.5;
      vOut[4] = yp_ + kickyp * 0.5;
      vOut[5] = zp;
      part.SetPosMom(vector6(vOut));
    }
  }
}

void TRKThin::Track(TRKOctupole* el, TRKBunch* bunch) { 
  const double strength = el->GetStrength();
  if (std::abs(strength)<=1e-12) {
    return Track((TRKDrift*)el,bunch);
  }

  const double h = el->GetLength()/trackingSteps;

  TRKBunch::iterator iter = bunch->begin();
  TRKBunch::iterator end = bunch->end();
  
  // initialise kick // only needed once
  double Kn = strength / 6;

  for (;iter!=end;++iter) {
    TRKParticle& part = *iter;
    // paraxial approximation ONLY!!
    if(iter== bunch->begin() && (std::abs(part.Zp())>0.99)&&(std::abs(strength)<1.e-6)) {
      TRKHybrid hybrid(trackingSteps);
      return hybrid.Track(el,bunch);
    }
    
    for (int i=0; i<trackingSteps; i++) {

      double x0 = part.X();
      double y0 = part.Y();
      double z0 = part.Z();
      double xp = part.Xp();
      double yp = part.Yp();
      double zp = part.Zp();

      // adapted from PLACET, element_thin_lens

      // calculate particle kick
      std::complex<double> Kick = Kn * std::pow(std::complex<double>(x0, y0),3);
      double kickxp = -real(Kick);
      double kickyp = +imag(Kick);

      // apply kick, 2 steps
      double xp_ = xp + kickxp * 0.5;
      double yp_ = yp + kickyp * 0.5;
      
      double vOut[6];
      vOut[0] = x0 + xp_ * h;
      vOut[1] = y0 + yp_ * h;
      vOut[2] = z0 + (0.5e-6 * (xp_*xp_ + yp_*yp_) * h); // to be checked

      // calculate particle kick
      Kick = Kn * std::pow(std::complex<double>(vOut[0], vOut[1]),3);
      kickxp = -real(Kick);
      kickyp = +imag(Kick);

      vOut[3] = xp_ + kickxp * 0.5;
      vOut[4] = yp_ + kickyp * 0.5;
      vOut[5] = zp;
      part.SetPosMom(vector6(vOut));
    }
  }
}

void TRKThin::Track(TRKSolenoid* el, TRKBunch* bunch) { 
  Track((TRKDrift*)el,bunch);
}
