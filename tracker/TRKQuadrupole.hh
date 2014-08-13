#ifndef TRKQuadrupole_h
#define TRKQuadrupole_h

#include "TRKTrackingElement.hh"

class TRKQuadrupole : public TRKTrackingElement {
protected: 
  double strength;

public : 
  TRKQuadrupole();  
  TRKQuadrupole(double strength, TRKTrackingElement::TRKType type, int trackingSteps, std::string name, double length, double size_x, double size_y, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKQuadrupole();
  
};

#endif
