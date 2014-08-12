#include "TRKTrackingElement.hh"

TRKTrackingElement::TRKTrackingElement(TRKType typeIn, int trackingStepsIn, TRKElement &element) : 
  TRKElement(element), type(typeIn), trackingSteps(trackingStepsIn) {
}

TRKTrackingElement::TRKTrackingElement(TRKType typeIn, int trackingStepsIn, 
				       std::string nameIn, double lengthIn, 
				       double size_xIn, double size_yIn, 
				       TRKAperture *apertureIn, TRKPlacement *placementIn) : 
  TRKElement(nameIn, lengthIn, size_xIn, size_yIn, apertureIn, placementIn), type(typeIn), trackingSteps(trackingStepsIn)
{
}

TRKTrackingElement::~TRKTrackingElement() { 
}



void TRKTrackingElement::Track(const double vIn[], double vOut[], double h) {   
  // we need to check if h < length, perhaps here?

  if(type == TRKTrackingElement::thin) {
    ThinTrack(vIn,vOut,h); 
  }
  else if(type == TRKTrackingElement::thick) {
    HybridTrack(vIn,vOut,h); 
  }
  else if(type == TRKTrackingElement::hybrid) {
    ThickTrack(vIn,vOut,h);
  }
} 

void TRKTrackingElement::Track(const double vIn[], double vOut[]) {     
  // double dh = length/trackingSteps;
  // for(double h = 0;h<length;h=h+dh) { 
  //   Track(vIn, vOut, h);    
  // }

  double dh = length/trackingSteps;
  double vTemp[6];

  /// vTemp = vIn;
  for (int i=0; i<6; i++) {
    vTemp[i]=vIn[i];
  }

  for(int i=0; i<trackingSteps; i++) {
    Track(vTemp, vOut, dh);
    ///    vTemp = vOut;
    for (int i=0; i<6; i++) {
      vTemp[i]=vOut[i];
    }
  }
}
