#include "TRKDipole.hh"

#include <cmath>
#include <cfloat>
#include <cstdlib>
#include <limits>
#include "vector3.hh"
#include "vector6.hh"

#include "CLHEP/Units/SystemOfUnits.h"

TRKDipole::TRKDipole(double strengthIn, double bFieldIn, TRKTrackingElement::TRKType typeIn, int trackingStepsIn, std::string nameIn, double lengthIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKTrackingElement(typeIn, trackingStepsIn,nameIn,lengthIn,apertureIn,placementIn), strength(strengthIn), bField(bFieldIn), drift(NULL)
{
  drift = new TRKDrift(typeIn, trackingStepsIn,nameIn+"Drift",lengthIn,apertureIn,placementIn);
}

TRKDipole::~TRKDipole() {
  delete drift;
}

void TRKDipole::ThinTrack(const double /*vIn*/[], double /*vOut*/[], double /*h*/) {  

}

void TRKDipole::HybridTrack(const double vIn[], double vOut[], double h) {  
  if (std::abs(strength)<=1e-12) {
    return drift->Track(vIn,vOut,h);
  }

  double x0 = vIn[0];
  double y0 = vIn[1];
  double z0 = vIn[2];
  double xp = vIn[3];
  double yp = vIn[4];
  double zp = vIn[5];

  // adapted from BDSDipoleStepper.cc
  // 1) bfield
  // 2) kicker strength , overwrites first calculation?

  vector3 yhat(0.,1.,0.);

  vector3 vhat(vIn[3],vIn[4],vIn[5]);
  
  vector3 vnorm = vhat.cross(yhat);
  // TODO: double initMag = vhat.mag(); // not correct, need to be included in bfield in Factory
  double R = DBL_MAX;

  //  if(bField!=0)
    //    R=-(InitMag/CLHEP::GeV)/(0.299792458 * itsBField/CLHEP::tesla) * CLHEP::m;
  //  else
  //    R=DBL_MAX;

 // include the sign of the charge of the particles

  // if(  fPtrMagEqOfMot->FCof()<0) R*=-1.;
  // else if ( fPtrMagEqOfMot->FCof()==0) R=DBL_MAX;
  
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
  
  vector3 outPos(dx + vIn[0] + EndNomPath - NominalPath,
		 dy + vIn[1],
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
}

void TRKDipole::ThickTrack(const double /*vIn*/[], double /*vOut*/[], double /*h*/) {

}

std::ostream& operator<< (std::ostream &out, const TRKDipole &element) {
  return out << element.strength << " ";
}
