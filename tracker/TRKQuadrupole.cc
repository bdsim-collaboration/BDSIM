#include "TRKQuadrupole.hh"

#include <cmath>
#include <cstdlib>
#include "vector3.hh"
#include "vector6.hh"

TRKQuadrupole::TRKQuadrupole(double strengthIn, TRKTrackingElement::TRKType typeIn, int trackingStepsIn, std::string nameIn, double lengthIn, double size_xIn, double size_yIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKTrackingElement(typeIn, trackingStepsIn,nameIn,lengthIn,size_xIn,size_yIn,apertureIn,placementIn), strength(strengthIn), drift(NULL)
{
  drift = new TRKDrift(typeIn, trackingStepsIn,nameIn+"Drift",lengthIn,size_xIn,size_yIn,apertureIn,placementIn);
}

TRKQuadrupole::~TRKQuadrupole() {
  delete drift;
}

void TRKQuadrupole::ThinTrack(const double vIn[], double vOut[], double h) {  
  if (std::abs(strength)<=1e-12) {
    return drift->Track(vIn,vOut,h);
  }
  /// half Drift + Thin Kick + half drift
  static double halfLength = length/2;

  double hBefore = halfLength - vIn[2];
  double hAfter = vIn[2] + h - halfLength;

  if (hBefore < 0 || hAfter < 0) {
    drift->Track(vIn,vOut,h);
    return;
  }

  /// first drift distance  
  drift->Track(vIn,vOut,hBefore);

  vector6 vKickIn(vOut);
  vector6 vKickOut;
  /// thin kick
  ThinKick(vKickIn,vKickOut);

  double vTemp[6];
  vKickOut.setArray(vTemp);
  /// second drift distance
  drift->Track(vTemp,vOut,hAfter);

}

void TRKQuadrupole::ThinKick(const vector6& vIn, vector6& vOut) {

  vOut = vIn;

  // double charge, oh charge good point, should just add this to method signature
  double charge = 1 * TRK::e;
  double rigidity = std::abs(strength) * vIn.mom().mag() / charge; // to be checked

  double k = 1/std::sqrt(std::abs(rigidity));

  if (k>0) {
    vOut.plusmom(vector3(-vIn.X()*k*std::sin(k*length), vIn.Y()*k*std::sinh(k*length), 0.0));
  } else {
    vOut.plusmom(vector3(vIn.X()*k*std::sinh(k*length), -vIn.Y()*k*std::sin(k*length), 0.0));
  }
}

void TRKQuadrupole::HybridTrack(const double vIn[], double vOut[], double h) {  
  if (std::abs(strength)<=1e-12) {
    return drift->Track(vIn,vOut,h);
  }

  /// from BDSQuadStepper
  double x0 = vIn[0];
  double y0 = vIn[1];
  double z0 = vIn[2];
  double xp = vIn[3];
  double yp = vIn[4];
  double zp = vIn[5];
  
  vector3 rpp (-zp*x0, zp*y0, x0*xp - y0*yp);
  rpp = rpp * strength;

  double R_1 = rpp.mag();

  if (R_1 == 0) { // curvature 0
    return drift->Track(vIn,vOut,h);
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
    
  vOut[0] = x1;
  vOut[1] = y1;
  vOut[2] = z1;
    
  vOut[3] = x1p;
  vOut[4] = y1p;
  vOut[5] = z1p;
    
}

void TRKQuadrupole::ThickTrack(const double vIn[], double vOut[], double h) {
  vector6 vTemp(vIn);
  // double charge, oh charge good point, should just add this to method signature
  double charge = 1 * TRK::e;

  if (std::abs(strength)<=1e-12) {
    return drift->Track(vIn,vOut,h);
  }

  double rigidity = std::abs(strength) * vTemp.mom().mag() / charge; // to be checked
  double k = 1/std::sqrt(std::abs(rigidity));

  double c,s,ch,sh; 
  //  sincos(k*h,&s,&c);
  c = std::cos(k*h);
  s = std::sin(k*h);
  TRK::sincosh(k*h,sh,ch);

  if (k>0) {
    vOut[0] = c * vIn[0] + s * vIn[1] / k;
    vOut[1] = c * vIn[1] - s * vIn[0] * k;
    vOut[2] = ch * vIn[2] + sh * vIn[3] / k;
    vOut[3] = ch * vIn[3] + sh * vIn[2] * k;
  } else {
    vOut[0] = ch * vIn[0] + sh * vIn[1] / k;
    vOut[1] = ch * vIn[1] + sh * vIn[0] * k;
    vOut[2] = c * vIn[2] + s * vIn[3] / k;
    vOut[3] = c * vIn[3] - s * vIn[2] * k;
  }
  vOut[4] = vIn[4];
  vOut[5] = vOut[5];
}


