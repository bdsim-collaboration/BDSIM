#ifndef TRKQuadrupole_h
#define TRKQuadrupole_h

#include "TRKElement.hh"

class TRKQuadrupole : public TRKElement {
public : 
  TRKQuadrupole();  
  TRKQuadrupole(TRKElement* element, double strength);
  ~TRKQuadrupole();
  
private: 
  double strength;
};

#endif
