#ifndef TRKDrift_h
#define TRKDrift_h

#include "vector3.hh"
#include "TRKTrackingElement.hh"

class TRKDrift : public TRKTrackingElement { 
public :
  TRKDrift();
  TRKDrift(std::string name, double length, double size_x, double size_y, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKDrift(); 

  // Specific implementations of different tracking routines 
  void ThinTrack(const double vIn[], double vOut[], double h);
  void HybridTrack(const double vIn[], double vOut[], double h);
  void ThickTrack(const double vIn[], double vOut[], double h);
  
private : 
  
  
};

#endif
