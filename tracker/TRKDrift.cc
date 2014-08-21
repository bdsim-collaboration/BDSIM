#include "TRKDrift.hh"

//TRKDrift::TRKDrift() {}
TRKDrift::TRKDrift(TRKTrackingElement::TRKType typeIn, int trackingSteps, std::string nameIn, double lengthIn, double size_xIn, double size_yIn, 
		   TRKAperture *apertureIn, TRKPlacement *placementIn) :
  TRKTrackingElement(typeIn, trackingSteps, nameIn,lengthIn, size_xIn, size_yIn, apertureIn, placementIn)
{
  
}

TRKDrift::~TRKDrift() {}

void TRKDrift::ThinTrack(const double vIn[], double vOut[], double h) {  
  vector3 v0  = vector3(vIn[0],vIn[1],vIn[2]);
  vector3 vp0 = vector3(vIn[3],vIn[4],vIn[5]); 
  
  vector3 dv = vp0.unit()*h;
  vector3 v1 = v0 + dv;
  v1.setArray(vOut);
  vp0.setArray(&vOut[3]);
}

void TRKDrift::HybridTrack(const double vIn[], double vOut[], double h) {  
  ThinTrack(vIn, vOut, h);
}

void TRKDrift::ThickTrack(const double vIn[], double vOut[], double h) {
  ThinTrack(vIn, vOut, h);
}

