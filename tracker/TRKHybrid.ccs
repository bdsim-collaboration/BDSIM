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
#include "TRKHybrid.hh"

#include <cmath>
#include <complex>
#include <cstdlib>
#include <limits>
#include <vector>

#include "TRK.hh"
#include "TRKBunch.hh"
#include "TRKParticle.hh"
//#include "TRKDrift.hh"
#include "TRKSBend.hh"
#include "TRKRBend.hh"
#include "TRKQuadrupole.hh"
#include "TRKSextupole.hh"
#include "TRKOctupole.hh"

#include "BDSDebug.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "BDSGlobalConstants.hh"

TRKHybrid::TRKHybrid(int trackingStepsIn):
  TRKStrategy(trackingStepsIn)
{;}

TRKHybrid::~TRKHybrid()
{;}

void TRKHybrid::Track(TRKDrift* el, TRKBunch* bunch)
{
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << " Drift" << std::endl;
#endif

  // this drift method can be done in one go, trackingsteps not needed;
  const double h = el->GetLength()*1e6; //convet from m to um
  TRKBunch::iterator iter = bunch->begin();
  TRKBunch::iterator end  = bunch->end();

  for (;iter!=end;++iter) {
    TRKParticle& part = *iter;
    
    vector3 unit = part.Mom().unit();
    vector3 dv = unit * h / unit.Z();   
    
    part.SetPosMom(dv+part.Pos(),part.Mom());    
  }
}


void TRKHybrid::Track(TRKSBend* el, TRKBunch* bunch) {
  #ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << " SBend" << std::endl;
  #endif
    
  // transverse coords are in um
  // routine expects all values in m, converts back to transverse um after
  
  // Keep arc length in m for this routine
  // TODO account for multiple tracking steps
  const double arclength = el->GetLength();
  const double angle = el->GetAngle();
  
  // for low angles track as drift
  if (std::abs(angle)<=1e-12) {
    return Track((TRKDrift*)el,bunch);
  }
  
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << " step length per tracking step: " << arclength << " um" << std::endl;
  std::cout << __METHOD_NAME__ << " number of tracking steps: " << trackingSteps << std::endl;
#endif

  TRKBunch::iterator iter = bunch->begin();
  TRKBunch::iterator end = bunch->end();
  
  for (;iter!=end;++iter) 
  {
    TRKParticle& part = *iter;
    // Converted from um to m for this routine
    double x0 = part.X()/1e6;
    double y0 = part.Y()/1e6;
    double z0 = part.Z();
    double xp = part.Xp();
    double yp = part.Yp();
        
    // Reference: Wiedemann Particle Accelerator Physics (C.5)
    
    // Rigidity effects on B field (linearly scaling in theta)
    double relangle = angle * (nominalMomentum/part.P());
    
    // rho = arclength / angle
    double gyroradius = arclength / relangle;
    // kappa = 1/rho
    double kappa = 1/gyroradius;
    
    double c,s; 
    c = std::cos(relangle);
    s = std::sin(relangle);
    
    #ifdef TRKDEBUG
    std::cout <<"cos(theta) " << c << " sin(theta) " << s <<std::endl;
    #endif
    
    // The rightmost factors of 1e6 are to convert m back to um
    double vOut[6];
    
    vOut[0] = (c * x0 + gyroradius * s * xp) * 1e6;   //x1
    vOut[1] = (y0 + arclength * yp)          * 1e6;   //y1
    vOut[3] = -kappa * s * x0 + c * xp;               //xp1
    vOut[4] = yp;                                     //yp1
    
    vOut[5] =sqrt(1 - vOut[3]*vOut[3] - vOut[4]*vOut[4]); // z1p = sqrt(1- xp1^2 - yp1^2)
    
    // Convert h (m) to (um)
    double dz = arclength*1e6;
    vOut[2]=z0+dz;   
    
    part.SetPosMom(vector6(vOut));
  }
}

void TRKHybrid::Track(TRKRBend* el, TRKBunch* bunch) {
  #ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << " RBend" << std::endl;
  #endif
  
  // transverse coords are in um
  // routine expects all values in m, converts back to transverse um after
  
  // Keep chord length in m for this routine
  // TODO Trackingsteps not trivial to implement in this geometry
  const double chordlength = el->GetLength();
  const double angle = el->GetAngle();
  
  // for low angles track as drift
  if (std::abs(angle)<=1e-12) {
    return Track((TRKDrift*)el,bunch);
  }
  
  #ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << " step length per tracking step: " << chordlength << " um" << std::endl;
  std::cout << __METHOD_NAME__ << " number of tracking steps: " << trackingSteps << std::endl;
  #endif
  
  TRKBunch::iterator iter = bunch->begin();
  TRKBunch::iterator end = bunch->end();
  
  for (;iter!=end;++iter) 
  {
    TRKParticle& part = *iter;
    // Converted from um to m for this routine
    double x0 = part.X()/1e6;
    double y0 = part.Y()/1e6;
    double z0 = part.Z();
    double xp = part.Xp();
    double yp = part.Yp();
    
    double relangle = angle * (nominalMomentum/part.P());
    
    // Geometric relation from chord/angle
    double gyroradius = 0.5 * chordlength / std::sin(relangle*0.5);
    
    double s; 
    s = std::sin(relangle);
    
    #ifdef TRKDEBUG
    std::cout <<"sin(theta) " << s <<std::endl;
    #endif
    
    // The rightmost factors of 1e6 are to convert m back to um
    double vOut[6];
    vOut[0] = (x0 + gyroradius * s * xp) * 1e6; //x1
    vOut[1] = (y0 + gyroradius * s * yp) * 1e6; //y1
    vOut[3] = xp;                               //xp1
    vOut[4] = yp;                               //yp1
    
    vOut[5] =sqrt(1 - vOut[3]*vOut[3] - vOut[4]*vOut[4]); // z1p = sqrt(1- xp1^2 - yp1^2)
    
    // Convert h (m) to (um)
    double dz = chordlength*1e6;
    vOut[2]=z0+dz;   
    
    part.SetPosMom(vector6(vOut));
  }
}

void TRKHybrid::Track(TRKQuadrupole* el, TRKBunch* bunch) {
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << " Quadrupole" << std::endl;
#endif
  double strength = el->GetStrength();

  // note from TRKQuadrupole.hh strength is in T/m
  // transverse coords are in um
  // routine expects all values in m, converts back to transverse um after
  if (std::abs(strength)<=1e-12) {
    return Track((TRKDrift*)el,bunch);
  }
  
  // Keep h in m for this routine
  const double h = el->GetLength()/trackingSteps;
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << " step length per tracking step: " << h << " um" << std::endl;
  std::cout << __METHOD_NAME__ << " number of tracking steps: " << trackingSteps << std::endl;
#endif
  TRKBunch::iterator iter = bunch->begin();
  TRKBunch::iterator end = bunch->end();

  for (;iter!=end;++iter) {
    TRKParticle& part = *iter;
    for (int i=0; i<trackingSteps; i++) {

      /// from BDSQuadStepper
      // Converted from um to m for this routine
      double x0 = part.X()/1e6;
      double y0 = part.Y()/1e6;
      double z0 = part.Z();
      double xp = part.Xp();
      double yp = part.Yp();

      // Use paraxial approximation:
#ifdef TRKDEBUG
      std::cout << "paraxial approximation being used" << std::endl;
      
#endif
      
      double k = strength * (nominalMomentum/part.P()); // k in T/m
      
      double rootK=sqrt(std::abs(k));
      double rootKh=rootK*h;
      
      double c,s,ch,sh; 
      c = std::cos(rootKh);
      s = std::sin(rootKh);
      TRK::sincosh(rootKh,sh,ch);

#ifdef TRKDEBUG
      std::cout <<"k "<< k <<" rootk "<<rootK<<" rootKH "<<rootKh<<std::endl;
      std::cout <<"cos(rootKh) " << c << " sin(rootKh) " << s <<std::endl;
      std::cout <<"sincosh(rootKh) " << sh << " " << ch << std::endl; 
#endif
      
      // Match direction of rotation with BDSIM (negative particle tracking bug)
      if (part.Charge() < 0) 
      {
#ifdef TRKDEBUG
        std::cout << "Tracking negative particle, flipping k value" << std::endl;
#endif
        k = -k;
      }
        
      
      // The rightmost factors of 1e6 are to convert m back to um
      double vOut[6];
  	  if (k>0)
	    {
        vOut[0] = ((c * x0) + (s * xp / rootK)) * 1e6;    //x1
        vOut[1] = ((ch * y0) + (sh * yp / rootK)) * 1e6;  //y1
        vOut[3] = (- s * x0 * rootK) + (c * xp);          //xp1
        vOut[4] = (sh * y0 * rootK) + (ch * yp);          //yp1
	    }
  	  else
	    {
        vOut[0] = ((ch * x0) + (sh * xp / rootK)) * 1e6;  //x1
        vOut[1] = ((c * y0) + (s * yp / rootK)) * 1e6;    //y1
        vOut[3] = (sh * x0 * rootK) + (ch * xp);          //xp1
        vOut[4] = (- s * y0 * rootK) + (c * yp);          //yp1
	    }
      
  	  vOut[5] =sqrt(1 - vOut[3]*vOut[3] - vOut[4]*vOut[4]); // z1p = sqrt(1- xp1^2 - yp1^2)
      
      // Convert h (m) to (um)
	    double dz = h*1e6;
      vOut[2]=z0+dz;   
      
      part.SetPosMom(vector6(vOut));
    }
  }
}

void TRKHybrid::Track(TRKSextupole* el, TRKBunch* bunch)
{
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << " Sextupole" << std::endl;
#endif
  // TODO Implement sextupole tracking
  Track((TRKDrift*)el,bunch);
}

void TRKHybrid::Track(TRKOctupole* el, TRKBunch* bunch)
{
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << " Octupole" << std::endl;
#endif
  std::cout << "DANGER WILL ROBINSON - NOT FINISHED" << std::endl;
  Track((TRKDrift*)el,bunch);
}

void TRKHybrid::Track(TRKSolenoid* el, TRKBunch* bunch)
{
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << " Solenoid " << std::endl;
#endif
  std::cout << "DANGER WILL ROBINSON - NOT FINISHED" << std::endl;
  Track((TRKDrift*)el,bunch);
}

void TRKHybrid::Track(TRKKicker *el, TRKBunch *bunch) {}
