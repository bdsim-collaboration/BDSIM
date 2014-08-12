#include "TRKQuadrupole.hh"

#include "vector3.hh"

TRKQuadrupole::TRKQuadrupole(std::string name, double length, double size_x, double size_y, double str, TRKAperture *aperture, TRKPlacement *placement):
  TRKTrackingElement(TRKTrackingElement::thin,TRK::DEFAULT_TRACKING_STEPS,name,length,size_x,size_y,aperture,placement),
  strength(str)
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


