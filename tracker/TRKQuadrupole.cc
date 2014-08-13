#include "TRKQuadrupole.hh"

#include "vector3.hh"

TRKQuadrupole::TRKQuadrupole(double strengthIn, TRKTrackingElement::TRKType typeIn, int trackingStepsIn, std::string nameIn, double lengthIn, double size_xIn, double size_yIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKTrackingElement(typeIn, trackingStepsIn,nameIn,lengthIn,size_xIn,size_yIn,apertureIn,placementIn), strength(strengthIn)
{  
}
TRKQuadrupole::~TRKQuadrupole() {}

void TRKQuadrupole::ThinTrack(const double vIn[], double vOut[], double h) {  
  vector3 v0  = vector3(vIn[0],vIn[1],vIn[2]);
  vector3 vp0 = vector3(vIn[3],vIn[4],vIn[5]); 
  
  vector3 dv = vp0.unit()*h;
  vector3 v1 = v0 + dv;
  v1.setArray(vOut);
  
}

void TRKQuadrupole::HybridTrack(const double vIn[], double vOut[], double h) {  
  ThinTrack(vIn, vOut, h);
}

void TRKQuadrupole::ThickTrack(const double vIn[], double vOut[], double h) {
  ThinTrack(vIn, vOut, h);
}


