#ifndef TRKPlacement_h
#define TRKPlacement_h

class TRKPlacement {
public : 
  TRKPlacement();
  TRKPlacement(double x, double y, double z);
  ~TRKPlacement(); 
  
protected :
  double x;
  double y;
  double z;
};

#endif
