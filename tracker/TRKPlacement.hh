#ifndef TRKPlacement_h
#define TRKPlacement_h

#include "vector3.hh"

class TRKPlacement {
public : 
  TRKPlacement();
  TRKPlacement(double x, double y, double z);
  ~TRKPlacement(); 
  
  vector3 GetGlobal()const;

private :
  double x;
  double y;
  double z;
};

#endif
