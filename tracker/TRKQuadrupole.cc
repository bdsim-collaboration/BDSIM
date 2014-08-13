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

  /// thin lens kick, not taking into account tilt for the moment 

  // how to get these:
  // double momentum
  // double charge
  // double rigidity = momentum / eV / c

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


