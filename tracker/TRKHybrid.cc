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
#include "TRKDipole.hh"
#include "TRKQuadrupole.hh"
#include "TRKSextupole.hh"
#include "TRKOctupole.hh"

#include "BDSDebug.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "BDSGlobalConstants.hh"

TRKHybrid::TRKHybrid(int trackingStepsIn):
  TRKStrategy(trackingStepsIn)
{;}

TRKHybrid::~TRKHybrid() {
}

void TRKHybrid::Track(TRKDrift* el, TRKBunch* bunch) {
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << " Drift" << std::endl;
#endif

  // this drift method can be done in one go, trackingsteps not needed;
  const double h = el->GetLength()*1e6; //convet from m to um
  TRKBunchIter iter = bunch->begin();
  TRKBunchIter end  = bunch->end();

  for (;iter!=end;++iter) {
    TRKParticle& part = *iter;
    
    vector3 unit = part.Mom().unit();
    vector3 dv = unit * h / unit.Z();   
    
    part.SetPosMom(dv+part.Pos(),part.Mom());    
  }
}

void TRKHybrid::Track(TRKDipole* el, TRKBunch* bunch) {
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << " Dipole" << std::endl;
#endif
  double strength = el->GetStrength();
  //note from TRKDipole.hh strength is in MeV* urad
  if (std::abs(strength)<=1e-12) {
    return Track((TRKDrift*)el,bunch);
  }

  const double h = el->GetLength()*1e6/trackingSteps; //convert form m to um

  TRKBunchIter iter = bunch->begin();
  TRKBunchIter end = bunch->end();

  for (;iter!=end;++iter) {
    TRKParticle& part = *iter;
    for (int i=0; i<trackingSteps; i++) {

      /// from BDSQuadStepper
      double x0 = part.X();
      double y0 = part.Y();
      double z0 = part.Z();
      double xp = part.Xp();
      double yp = part.Yp();
      double zp = part.Zp();

      // adapted from BDSDipoleStepper.cc
      // 1) bfield
      // 2) kicker strength , overwrites first calculation?

      vector3 yhat(0.,1.,0.);

      vector3 vhat = part.Mom();

      vector3 vnorm = vhat.cross(yhat);
      // TODO: double initMag = vhat.mag(); // not correct, need to be included in bfield in Factory
      double R = std::numeric_limits<double>::max();

      //  if(bField!=0)
      //    R=-(InitMag/CLHEP::GeV)/(0.299792458 * itsBField/CLHEP::tesla) * CLHEP::m;
      //  else
      //    R=std::numeric_limits<double>::max();

      // include the sign of the charge of the particles

      // if(  fPtrMagEqOfMot->FCof()<0) R*=-1.;
      // else if ( fPtrMagEqOfMot->FCof()==0) R=std::numeric_limits<double>::max();

      double theta   = h/R;
      double cosT_ov_2, sinT_ov_2, cosT, sinT;
      cosT_ov_2 = cos(theta/2);
      sinT_ov_2 = sin(theta/2);

      cosT = (cosT_ov_2 * cosT_ov_2)- (sinT_ov_2 * sinT_ov_2);
      sinT = 2 * cosT_ov_2 * sinT_ov_2;

      //BDSLocalRadiusOfCurvature=R;

      //  itsDist=fabs(R)*(1.-CosT_ov_2);

      vector3 dPos = (vhat*sinT + vnorm*(1-cosT)) * R;

      // strength

      double vOut[6];
      vOut[0] = x0+dPos.X();
      vOut[1] = y0+dPos.Y();
      vOut[2] = z0+dPos.Z();

      vOut[3] = cosT*vhat.X() + sinT*vnorm.X();
      vOut[4] = cosT*vhat.Y() + sinT*vnorm.Y();
      vOut[5] = cosT*vhat.Z() + sinT*vnorm.Z();

      if(fabs(strength)<std::numeric_limits<double>::epsilon()) {  // no gradient
	return;
      }

      double x1,x1p,y1,y1p,z1p;

      /** TODO   NOMR should go to factory
	  double NomEnergy = BDSGlobalConstants::Instance()->BeamTotalEnergy();
	  double NomR = -(NomEnergy/CLHEP::GeV)/(0.299792458 * bField/CLHEP::tesla) * CLHEP::m;

	  double NominalPath = sqrt(NomR*NomR - z0*z0) - fabs(NomR)*cos(itsAngle/2);

	  double EndNomPath = sqrt(NomR*NomR - vOut[2]*vOut[2]) - fabs(NomR)*cos(itsAngle/2);
      */
      //  double NomR = 0.0; // temp
      double NominalPath =0.0; // temp
      double EndNomPath =0.0;

      if(R<0)
	{
	  NominalPath*=-1;
	  EndNomPath*=-1;
	}

      x0=x0 - NominalPath;

      // TODO rotate!
      //double theta_in = asin(z0/NomR);
      //LocalRp.rotateY(-theta_in);


      double rootK=sqrt(fabs(strength*zp));
      double rootKh=rootK*h*zp;
      double X11,X12,X21,X22;
      double Y11,Y12,Y21,Y22;

      if (strength>0)
	{
	  X11= cos(rootKh);
	  X12= sin(rootKh)/rootK;
	  X21=-fabs(strength)*X12;
	  X22= X11;

	  Y11= cosh(rootKh);
	  Y12= sinh(rootKh)/rootK;
	  Y21= fabs(strength)*Y12;
	  Y22= Y11;
	}
      else // if (strength<0)
	{
	  X11= cosh(rootKh);
	  X12= sinh(rootKh)/rootK;
	  X21= fabs(strength)*X12;
	  X22= X11;

	  Y11= cos(rootKh);
	  Y12= sin(rootKh)/rootK;
	  Y21= -fabs(strength)*Y12;
	  Y22= Y11;
	}

      x1      = X11*x0 + X12*xp;
      x1p     = X21*x0 + X22*xp;

      y1      = Y11*y0 + Y12*yp;
      y1p     = Y21*y0 + Y22*yp;

      z1p=sqrt(1 - x1p*x1p -y1p*y1p);

      double dx=x1-x0;
      double dy=y1-y0;
      // Linear chord length

      vector3 outPos(dx + part.X() + EndNomPath - NominalPath,
		     dy + part.Y(),
		     vOut[2]);

      vector3 outDir(x1p, y1p, z1p);

      //  TODO
      // outDir.rotateY(theta_in);
      // outDir.rotateY(-h/R);
      //  GlobalTangent*=InitMag;

      vOut[0] = outPos.X();
      vOut[1] = outPos.Y();
      vOut[2] = outPos.Z();

      vOut[3] = outDir.X();
      vOut[4] = outDir.Y();
      vOut[5] = outDir.Z();
      part.SetPosMom(vector6(vOut));
    }
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

  TRKBunchIter iter = bunch->begin();
  TRKBunchIter end = bunch->end();
  
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
  
  TRKBunchIter iter = bunch->begin();
  TRKBunchIter end = bunch->end();
  
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
  TRKBunchIter iter = bunch->begin();
  TRKBunchIter end = bunch->end();

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

void TRKHybrid::Track(TRKSextupole* el, TRKBunch* bunch) {
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << " Sextupole" << std::endl;
#endif
  // TODO Implement sextupole tracking
  Track((TRKDrift*)el,bunch);
}

void TRKHybrid::Track(TRKOctupole* el, TRKBunch* bunch) {
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << " Octupole" << std::endl;
#endif
  std::cout << "DANGER WILL ROBINSON - NOT FINISHED" << std::endl;
  Track((TRKDrift*)el,bunch);
}

void TRKHybrid::Track(TRKSolenoid* el, TRKBunch* bunch) {
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << " Solenoid " << std::endl;
#endif
  std::cout << "DANGER WILL ROBINSON - NOT FINISHED" << std::endl;
  Track((TRKDrift*)el,bunch);
}
