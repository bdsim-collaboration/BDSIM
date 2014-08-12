#include "TRKDrift.hh"

TRKDrift::TRKDrift() {}
TRKDrift::TRKDrift(std::string nameIn, double lengthIn, double size_xIn, double size_yIn, 
		   TRKApertureInterface *apertureIn, TRKPlacement *placementIn) :
  TRKElement(nameIn,lengthIn, size_xIn, size_yIn, apertureIn, placementIn)
{
  
}

TRKDrift::~TRKDrift() {}

void TRKDrift::Track(const double vIn[], double vOut[], double h) {   
  if(type == TRKTrackingInterface::thin) {
    ThinTrack(vIn,vOut,h); 
  }
  else if(type == TRKTrackingInterface::thick) {
    HybridTrack(vIn,vOut,h); 
  }
  else if(type == TRKTrackingInterface::hybrid) {
    ThickTrack(vIn,vOut,h);
  }
} 

void TRKDrift::Track(const double vIn[], double vOut[]) {     
  double dh = length/trackingSteps;
  for(double h = 0;h<length;h=h+dh) { 
    Track(vIn, vOut, h);    
  }
}

void TRKDrift::ThinTrack(const double vIn[], double vOut[], double h) {  
}

void TRKDrift::HybridTrack(const double vIn[], double vOut[], double h) {  
  ThinTrack(vIn, vOut, h);
}

void TRKDrift::ThickTrack(const double vIn[], double vOut[], double h) {
  ThinTrack(vIn, vOut, h);
}

