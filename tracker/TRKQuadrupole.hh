#ifndef TRKQuadrupole_h
#define TRKQuadrupole_h

#include "TRKElement.hh"

class TRKQuadrupole : public TRKElement {
private: 
  double strength;

public : 
  TRKQuadrupole();  
  TRKQuadrupole(std::string name, double length, double size_x, double size_y, double strength, TRKApertureInterface *aperture, TRKPlacement *placement);
  ~TRKQuadrupole();
  
  virtual void Track(const double vIn[], double vOut[], double h);
  virtual void Track(const double vIn[], double vOut[]);

};

#endif
