#include "TRKHybrid.hh"

#include <cmath>
#include <complex>
#include <cstdlib>
#include <limits>
#include <vector>

#include "TRKBunch.hh"
#include "TRKParticle.hh"
#include "TRKDipole.hh"
#include "TRKQuadrupole.hh"
#include "TRKSextupole.hh"
#include "TRKOctupole.hh"

TRKHybrid::TRKHybrid(int trackingStepsIn) : 
  TRKStrategy(trackingStepsIn) {
}

TRKHybrid::~TRKHybrid() { 
}

void TRKHybrid::Track(TRKDrift* el, TRKBunch* bunch) { 
  const double h = el->GetLength()/trackingSteps;

  TRKBunchIter iter = bunch->begin();
  TRKBunchIter end  = bunch->end();

  for (;iter!=end;++iter) {
    TRKParticle& part = *iter;
    for (int i=0; i<trackingSteps; i++) {
      vector3 dv = part.Mom().unit()*h;
      part.PlusPos(dv);
    }
  }
}

void TRKHybrid::Track(TRKDipole* el, TRKBunch* bunch) { 
  double strength = el->GetStrength();
  if (std::abs(strength)<=1e-12) {
    return Track((TRKDrift*)el,bunch);
  }

  const double h = el->GetLength()/trackingSteps;

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
	  double NomEnergy = BDSGlobalConstants::Instance()->GetBeamTotalEnergy();
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

void TRKHybrid::Track(TRKQuadrupole* el, TRKBunch* bunch) { 
  double strength = el->GetStrength();
  if (std::abs(strength)<=1e-12) {
    return Track((TRKDrift*)el,bunch);
  }

  const double h = el->GetLength()/trackingSteps;

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
  
      vector3 rpp (-zp*x0, zp*y0, x0*xp - y0*yp);
      rpp = rpp * strength;

      double R_1 = rpp.mag();

      if (R_1 == 0) { // curvature 0
	/// TODO single particle tracking?
	//	return drift->Track(vIn,vOut,h);
      }

      double h2=h*h;

      double x1,x1p,y1,y1p,z1,z1p;

      double R=1./R_1;
      // chord distance (simple quadratic approx)
      //  itsDist= h2/(8*R);
      // check for paraxial approximation:
      if((std::abs(zp)>0.99)&&(std::abs(strength)<1.e-6))
	{
#ifdef TRKDEBUG
	  std::cout << "paraxial approximation being used" << std::endl;
#endif
	  double rootK=sqrt(std::abs(strength*zp));
	  double rootKh=rootK*h*zp;
	  double X11,X12,X21,X22;
	  double Y11,Y12,Y21,Y22;
      
	  if (strength>0)
	    {
	      X11= std::cos(rootKh);
	      X12= std::sin(rootKh)/rootK;
	      X21=-std::abs(strength)*X12;
	      X22= X11;
	    
	      Y11= std::cosh(rootKh);
	      Y12= std::sinh(rootKh)/rootK;
	      Y21= std::abs(strength)*Y12;
	      Y22= Y11;
	    }
	  else //if (strength<0)
	    {
	      X11= std::cosh(rootKh);
	      X12= std::sinh(rootKh)/rootK;
	      X21= std::abs(strength)*X12;
	      X22= X11;
	    
	      Y11= std::cos(rootKh);
	      Y12= std::sin(rootKh)/rootK;
	      Y21= -std::abs(strength)*Y12;
	      Y22= Y11;
	    }

	  x1 = X11*x0 + X12*xp;    
	  x1p= X21*x0 + X22*xp;
	
	  y1 = Y11*y0 + Y12*yp;    
	  y1p= Y21*y0 + Y22*yp;
	
	  z1p=sqrt(1 - x1p*x1p -y1p*y1p);

	  double dx=x1-x0;
	  double dy=y1-y0;
	
	  // Linear chord length
	  double dR2=dx*dx+dy*dy;
	  double dz=std::sqrt(h2*(1.-h2/(12*R*R))-dR2);
	      
	  // check for precision problems
	  double ScaleFac=(dx*dx+dy*dy+dz*dz)/h2;
	  if(ScaleFac>1.0000001)
	    {
	      ScaleFac=std::sqrt(ScaleFac);
	      dx/=ScaleFac;
	      dy/=ScaleFac;
	      dz/=ScaleFac;
	      x1=x0+dx;
	      y1=y0+dy;
	    }
	  z1=z0+dz;
	}
      else
	// perform local helical steps (paraxial approx not safe)
	{
	  // simple quadratic approx:	      
	  double quadX= - strength*x0*zp;
	  double quadY=   strength*y0*zp;
	  double quadZ=   strength*(x0*xp - y0*yp);
	      
	  // determine maximum curvature:
	  double maxCurv=std::max(std::abs(quadX),std::abs(quadY));
	  maxCurv=std::max(maxCurv,std::abs(quadZ));
	
	  x1 = x0 + h*xp + quadX*h2/2;
	  y1 = y0 + h*yp + quadY*h2/2; 
	  z1 = z0 + h*zp + quadZ*h2/2;
	
	  x1p = xp + quadX*h;
	  y1p = yp + quadY*h;
	  z1p = zp + quadZ*h;
	
	  // estimate parameters at end of step:
	  double quadX_end= - strength*x1*z1p;
	  double quadY_end=   strength*y1*z1p;
	  double quadZ_end=   strength*(x1*x1p - y1*y1p);
	      
	  // determine maximum curvature:
	  maxCurv=std::max(maxCurv,std::abs(quadX_end));
	  maxCurv=std::max(maxCurv,std::abs(quadY_end));
	  maxCurv=std::max(maxCurv,std::abs(quadZ_end));
	
	  //      itsDist=maxCurv*h2/4.;
	
	  // use the average:
	  double quadX_av=(quadX+quadX_end)/2;
	  double quadY_av=(quadY+quadY_end)/2;
	  double quadZ_av=(quadZ+quadZ_end)/2;
	
	  double x_prime_av=(xp + x1p)/2;
	  double y_prime_av=(yp + y1p)/2;
	  double z_prime_av=(zp + z1p)/2;
	
	  x1 = x0 + h*x_prime_av + quadX_av * h2/2;
	  y1 = y0 + h*y_prime_av + quadY_av * h2/2; 
	  z1 = z0 + h*z_prime_av + quadZ_av * h2/2;
	
	  x1p = xp + quadX_av*h;
	  y1p = yp + quadY_av*h;
	  z1p = zp + quadZ_av*h;
	
	  double dx = (x1-x0);
	  double dy = (y1-y0);
	  double dz = (z1-z0);
	  double chord2 = dx*dx + dy*dy + dz*dz;
	  if(chord2>h2)
	    {
	      double hnew=h*sqrt(h2/chord2);
	      x1=x0 + hnew*x_prime_av + quadX_av * hnew*hnew/2;
	      y1=y0 + hnew*y_prime_av + quadY_av * hnew*hnew/2; 
	      z1=z0 + hnew*z_prime_av + quadZ_av * hnew*hnew/2;
	    
	      x1p=xp + quadX_av*hnew;
	      y1p=yp + quadY_av*hnew;
	      z1p=zp + quadZ_av*hnew;
	    }
	}
    
      double vOut[6];
      vOut[0] = x1;
      vOut[1] = y1;
      vOut[2] = z1;
    
      vOut[3] = x1p;
      vOut[4] = y1p;
      vOut[5] = z1p;
      part.SetPosMom(vector6(vOut));
    }
  }
}

void TRKHybrid::Track(TRKSextupole* el, TRKBunch* bunch) { 
  double strength = el->GetStrength();
  if (std::abs(strength)<=1e-12) {
    return Track((TRKDrift*)el,bunch);
  }

  const double h = el->GetLength()/trackingSteps;

  TRKBunchIter iter = bunch->begin();
  TRKBunchIter end = bunch->end();
  
  for (;iter!=end;++iter) {
    TRKParticle& part = *iter;
    for (int i=0; i<trackingSteps; i++) {
      double x0 = part.X();
      double y0 = part.Y();
      double z0 = part.Z();
      double xp = part.Xp();
      double yp = part.Yp();
      double zp = part.Zp();

      // from BDSSextStepper
      // Evaluate field at the approximate midpoint of the step.
      x0=x0+xp*h/2;
      y0=y0+yp*h/2;
       
      double x02My02=(x0*x0-y0*y0);

      // local r'' (for curvature)
      vector3 LocalRpp(-zp*x02My02, 2*zp*x0*y0, xp*x02My02-2*yp*x0*y0);

      //G4cout << "LocalRpp: " <<LocalRpp<< G4endl;

      LocalRpp*=strength/2; // 2 is actually a 2! factor.
      // determine effective curvature
      double R_1 = LocalRpp.mag();

      if (R_1==0) {
	// TODO
	//return drift->Track(vIn,vOut,h);
      }

      double h2=h*h;
      // chord distance (simple quadratic approx)
      //itsDist= h2*R_1/8;
  
      // Save for Synchrotron Radiation calculations:
      //BDSLocalRadiusOfCurvature=1./R_1;
	   
      double dx=xp*h + LocalRpp.X()*h2 /2.; 
      double dy=yp*h + LocalRpp.Y()*h2 /2.;
  
      double dz=sqrt(h2*(1.-h2*R_1*R_1/12)-dx*dx-dy*dy);
      // check for precision problems
      double ScaleFac=(dx*dx+dy*dy+dz*dz)/h2;
      if(ScaleFac>1.0000001)
	{
	  ScaleFac=sqrt(ScaleFac);
	  dx/=ScaleFac;
	  dy/=ScaleFac;
	  dz/=ScaleFac;
	}

      double vOut[6];
      vOut[0] = x0+dx;
      vOut[1] = y0+dy;
      vOut[2] = z0+dz;

      vOut[3] += h*LocalRpp.X();
      vOut[4] += h*LocalRpp.Y();
      vOut[5] += h*LocalRpp.Z();
      part.SetPosMom(vector6(vOut));
    }
  }
}

void TRKHybrid::Track(TRKOctupole* /*el*/, TRKBunch* /*bunch*/) { 
}
