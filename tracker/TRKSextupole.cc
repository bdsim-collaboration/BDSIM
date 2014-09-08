#include "TRKSextupole.hh"

#include <cmath>
#include <cstdlib>
#include "vector3.hh"
#include "vector6.hh"

TRKSextupole::TRKSextupole(double strengthIn, TRKTrackingElement::TRKType typeIn, int trackingStepsIn, std::string nameIn, double lengthIn, double size_xIn, double size_yIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKTrackingElement(typeIn, trackingStepsIn,nameIn,lengthIn,size_xIn,size_yIn,apertureIn,placementIn), strength(strengthIn), drift(NULL)
{
  drift = new TRKDrift(typeIn, trackingStepsIn,nameIn+"Drift",lengthIn,size_xIn,size_yIn,apertureIn,placementIn);
}

TRKSextupole::~TRKSextupole() {
  delete drift;
}

void TRKSextupole::ThinTrack(const double vIn[], double vOut[], double h)
{
  if (std::abs(strength)<=1e-12) {
    return drift->Track(vIn,vOut,h);
  }


}

void TRKSextupole::ThinKick(const vector6& /*vIn*/, vector6& /*vOut*/)
{

}

void TRKSextupole::HybridTrack(const double vIn[], double vOut[], double h)
{
  if (std::abs(strength)<=1e-12) {
    return drift->Track(vIn,vOut,h);
  }

  double x0 = vIn[0];
  double y0 = vIn[1];
  double z0 = vIn[2];
  double xp = vIn[3];
  double yp = vIn[4];
  double zp = vIn[5];

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
    return drift->Track(vIn,vOut,h);
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
  
  vOut[0] = x0+dx;
  vOut[1] = y0+dy;
  vOut[2] = z0+dz;

  vOut[3] += h*LocalRpp.X();
  vOut[4] += h*LocalRpp.Y();
  vOut[5] += h*LocalRpp.Z();
}

void TRKSextupole::ThickTrack(const double vIn[], double vOut[], double h)
{
  if (std::abs(strength)<=1e-12) {
    return drift->Track(vIn,vOut,h);
  }
}

std::ostream& operator<< (std::ostream &out, const TRKSextupole &element) {
  return out << element.strength << " ";
}
