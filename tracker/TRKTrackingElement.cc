#include "TRKTrackingElement.hh"

#include <iostream>

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
  // we need to check if h < length, perhaps here? -> better inside aperturecheck or something

#ifdef TRKDEBUG
  std::cout << "TRKTrackingElement " << name << " : initial point in local coordinates:" << std::endl
	 << " x= " << vIn[0] << std::endl
	 << " y= " << vIn[1] << std::endl
	 << " z= " << vIn[2] << std::endl
	 << " x'= " << vIn[3] << std::endl
	 << " y'= " << vIn[4] << std::endl
	 << " z'= " << vIn[5] << std::endl
	 << std::endl; 
#endif

  if(type == TRKTrackingElement::thin) {
    ThinTrack(vIn,vOut,h); 
  }
  else if(type == TRKTrackingElement::hybrid) {
    HybridTrack(vIn,vOut,h); 
  }
  else if(type == TRKTrackingElement::thick) {
    ThickTrack(vIn,vOut,h);
  }

  if (aperture) {
    bool lost = aperture->CheckAperture(vOut);
    if (lost) {
      // do something;
      // track with aperture check?
      // storeParticle();
    }
  }

#ifdef TRKDEBUG
  std::cout << "TRKTrackingElement " << name << " : output point in local coordinates:" << std::endl
	 << " x= " << vOut[0] << std::endl
	 << " y= " << vOut[1] << std::endl
	 << " z= " << vOut[2] << std::endl
	 << " x'= " << vOut[3] << std::endl
	 << " y'= " << vOut[4] << std::endl
	 << " z'= " << vOut[5] << std::endl
	 << std::endl; 
#endif
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
