#ifndef TRKParticle_h
#define TRKParticle_h

#include <string>

class TRKParticle { 
private : 
  TRKParticle();
  TRKParticle(std::string nameIn, double massIn, double charge);
  ~TRKParticle();

public : 
  std::string name; 
  double mass;
  double charge;   
};

#endif
