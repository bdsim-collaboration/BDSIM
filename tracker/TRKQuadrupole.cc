#include "TRKQuadrupole.hh"

#include <cmath>
#include "vector3.hh"

TRKQuadrupole::TRKQuadrupole(double strengthIn, TRKTrackingElement::TRKType typeIn, int trackingStepsIn, std::string nameIn, double lengthIn, double size_xIn, double size_yIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKTrackingElement(typeIn, trackingStepsIn,nameIn,lengthIn,size_xIn,size_yIn,apertureIn,placementIn), strength(strengthIn)
{  
}
TRKQuadrupole::~TRKQuadrupole() {}

void TRKQuadrupole::ThinTrack(const double vIn[], double vOut[], double h) {  
  double vTemp[6];

  /// half step
  //  TRKDrift::ThinTrack(vIn,vTemp,h/2);


  // if v[3]+h is more than length/2.0 apply drift of length/2.0 - v[3] , then thin lens, then drift of  v[3] + h - length/2.0 
  // if v[3] < length/2.0 && v[3]+h is less than length/2.0 apply drift only 
  
  vector3 pos = vector3(vIn[0],vIn[1],vIn[2]);
  vector3 mom = vector3(vIn[3],vIn[4],vIn[5]);
  double  momMag = mom.mag();

  /// thin lens kick, not taking into account tilt for the moment 
  // won't all the thin trackers require a TRKDrift? 

  // how to get these:
  // double momentum mag(vIn[3],vIn[4],vIn[5]). use vector3 class 
  // double charge, oh charge good point, should just add this to method signature
  // double rigidity = momentum / eV / c, constants are easy

  double k = std::sqrt(std::abs(strength));

  if (k>0) {
    vTemp[4] += -vTemp[1]*k*std::sin(k*h); 
    vTemp[5] +=  vTemp[1]*k*std::sinh(k*h);
  } else {
    vTemp[4] +=  vTemp[1]*k*std::sinh(k*h); 
    vTemp[5] += -vTemp[1]*k*std::sin(k*h);
  }

  /// half step
  //  TRKDrift::ThinTrack(vTemp,vOut,h/2);
}

void TRKQuadrupole::HybridTrack(const double vIn[], double vOut[], double h) {  
  ThinTrack(vIn, vOut, h);
}

void TRKQuadrupole::ThickTrack(const double vIn[], double vOut[], double h) {
  ThinTrack(vIn, vOut, h);
}


