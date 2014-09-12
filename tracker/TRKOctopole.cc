#include "TRKOctopole.hh"

#include <cmath>
#include <complex>
#include <cstdlib>
#include "vector3.hh"
#include "vector6.hh"

TRKOctopole::TRKOctopole(double strengthIn, TRKTrackingElement::TRKType typeIn, int trackingStepsIn, std::string nameIn, double lengthIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKTrackingElement(typeIn, trackingStepsIn,nameIn,lengthIn,apertureIn,placementIn), strength(strengthIn), drift(NULL)
{
  drift = new TRKDrift(typeIn, trackingStepsIn,nameIn+"Drift",lengthIn,apertureIn,placementIn);
}

TRKOctopole::~TRKOctopole() {
  delete drift;
}

void TRKOctopole::ThinTrack(const double vIn[], double vOut[], double h) {  
  if (std::abs(strength)<=1e-12) {
    return drift->Track(vIn,vOut,h);
  }

  double x0 = vIn[0];
  double y0 = vIn[1];
  double z0 = vIn[2];
  double xp = vIn[3];
  double yp = vIn[4];
  double zp = vIn[5];

  // adapted from PLACET, element_thin_lens
  // paraxial approximation ONLY!!
  if((std::abs(zp)>0.99)&&(std::abs(strength)<1.e-6)) {
    return HybridTrack(vIn,vOut,h);
  }

  // initialise kick // only needed once
  double Kn = strength / 6;

  // calculate particle kick
  std::complex<double> Kick = Kn * std::pow(std::complex<double>(x0, y0),3);
  double kickxp = -real(Kick);
  double kickyp = +imag(Kick);

  // apply kick, 2 steps
  double xp_ = xp + kickxp * 0.5;
  double yp_ = yp + kickyp * 0.5;

  vOut[0] = x0 + xp_ * h;
  vOut[1] = y0 + yp_ * h;
  vOut[2] = z0 + (0.5e-6 * (xp_*xp_ + yp_*yp_) * h); // to be checked

  // calculate particle kick
  Kick = Kn * std::pow(std::complex<double>(vOut[0], vOut[1]),3);
  kickxp = -real(Kick);
  kickyp = +imag(Kick);

  vOut[3] = xp_ + kickxp * 0.5;
  vOut[4] = yp_ + kickyp * 0.5;
  // vOut[5] = zp;

}

void TRKOctopole::HybridTrack(const double vIn[], double vOut[], double h) {  
  if (std::abs(strength)<=1e-12) {
    return drift->Track(vIn,vOut,h);
  }
  double x0 = vIn[0];
  double y0 = vIn[1];
  double z0 = vIn[2];
  double xp = vIn[3];
  double yp = vIn[4];
  double zp = vIn[5];

  double y3fac=y0*(y0*y0-3*x0*x0);
  double x3fac=x0*(x0*x0-3*y0*y0);
      
  // local r'' (for curvature)
  //ThreeVector LocalRpp;
  // extra minus signs were because x,y_machine = - x_,-y_geant_world
  // New CVS version of BDSIM uses +x, +y in geant world
  /*
    LocalRpp.setX(zp*x3fac);
    LocalRpp.setY(zp*y3fac);
    LocalRpp.setZ(- xp*x3fac - yp*y3fac);
  */
  vector3 LocalRpp(-zp*x3fac, -zp*y3fac, xp*x3fac + yp*y3fac);
  
  LocalRpp*=strength/6; // 6 is actually a 3! factor.;

  // determine effective curvature
  double R_1 = LocalRpp.mag();
  if (R_1==0) {
    return drift->Track(vIn,vOut,h);
  }

  // Save for Synchrotron Radiation calculations:
  //BDSLocalRadiusOfCurvature=1/R_1;

  // chord distance (simple quadratic approx)
  double h2=h*h;
  //itsDist= h2*R_1/8;
  
  double dx=xp*h + LocalRpp.X()*h2/2; 
  double dy=yp*h + LocalRpp.Y()*h2/2; 
  
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

  vOut[0] = x0+dx;
  vOut[1] = y0+dy;
  vOut[2] = z0+dz;

  vOut[3] += h*LocalRpp.X();
  vOut[4] += h*LocalRpp.Y();
  vOut[5] += h*LocalRpp.Z();
}

void TRKOctopole::ThickTrack(const double vIn[], double vOut[], double h) {
  if (std::abs(strength)<=1e-12) {
    return drift->Track(vIn,vOut,h);
  }

}

std::ostream& operator<< (std::ostream &out, const TRKOctopole &element) {
  return out << element.strength << " ";
}
