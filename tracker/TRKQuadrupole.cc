#include "TRKQuadrupole.hh"

#include <cmath>
#include <cstdlib>
#include "vector3.hh"
#include "vector6.hh"

TRKQuadrupole::TRKQuadrupole(double strengthIn, TRKTrackingElement::TRKType typeIn, int trackingStepsIn, std::string nameIn, double lengthIn, double size_xIn, double size_yIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKTrackingElement(typeIn, trackingStepsIn,nameIn,lengthIn,size_xIn,size_yIn,apertureIn,placementIn), strength(strengthIn), thinDrift(NULL)
{
  if (type == TRKTrackingElement::thin) {
    thinDrift = new TRKDrift(typeIn, trackingStepsIn,nameIn,lengthIn,size_xIn,size_yIn,apertureIn,placementIn);
  }
}

TRKQuadrupole::~TRKQuadrupole() {
  delete thinDrift;
}

void TRKQuadrupole::ThinTrack(const double vIn[], double vOut[], double h) {  
  /// half Drift + Thin Kick + half drift
  static double halfLength = length/2;

  double hBefore = halfLength - vIn[2];
  double hAfter = vIn[2] + h - halfLength;

  if (hBefore < 0 || hAfter < 0) {
    thinDrift->Track(vIn,vOut,h);
    return;
  }

  /// first drift distance  
  thinDrift->Track(vIn,vOut,hBefore);

  vector6 vKickIn(vOut);
  vector6 vKickOut;
  /// thin kick
  ThinKick(vKickIn,vKickOut);

  double vTemp[6];
  vKickOut.setArray(vTemp);
  /// second drift distance
  thinDrift->Track(vTemp,vOut,hAfter);

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
  ThinTrack(vIn, vOut, h);
}

void TRKQuadrupole::ThickTrack(const double vIn[], double vOut[], double h) {
  vector6 vTemp(vIn);
  // double charge, oh charge good point, should just add this to method signature
  double charge = 1 * TRK::e;
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


