#ifndef TRKQuadrupole_h
#define TRKQuadrupole_h

#include "TRKTrackingElement.hh"

class TRKQuadrupole : public TRKTrackingElement {
private: 
  double strength;

public : 
  TRKQuadrupole();  
  TRKQuadrupole(std::string name, double length, double size_x, double size_y, double strength, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKQuadrupole();
  
  virtual void Track(const double vIn[], double vOut[], double h);
  virtual void Track(const double vIn[], double vOut[]);

};

#endif
