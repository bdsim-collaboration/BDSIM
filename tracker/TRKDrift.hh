#ifndef TRKDrift_h
#define TRKDrift_h

#include "TRKElement.hh"

class TRKDrift : public TRKElement { 
public :
  TRKDrift();
  TRKDrift(std::string name, double length, double size_x, double size_y, TRKApertureInterface *aperture, TRKPlacement *placement);
  ~TRKDrift(); 
  void Track(const double vIn[], double vOut[], double h); 
  void Track(const double vIn[], double vOut[]);

private : 
  
  
};

#endif
