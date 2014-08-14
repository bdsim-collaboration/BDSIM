#include "TRKQuadrupole.hh"

#include <cmath>
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
  
  // double momentum mag(vIn[3],vIn[4],vIn[5]). use vector3 class 
  // double charge, oh charge good point, should just add this to method signature
  // double rigidity = momentum / eV / c, constants are easy

  double k = std::sqrt(std::abs(strength));
}


