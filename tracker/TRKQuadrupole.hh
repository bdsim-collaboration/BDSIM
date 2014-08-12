#ifndef TRKQuadrupole_h
#define TRKQuadrupole_h

#include "TRKTrackingElement.hh"

class TRKQuadrupole : public TRKTrackingElement {
private: 
  double strength;

public : 
  TRKQuadrupole(std::string name, double length, double size_x, double size_y, double strength, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKQuadrupole();
  
protected:
  virtual void ThinTrack(const double vIn[], double vOut[], double h);
  virtual void HybridTrack(const double vIn[], double vOut[], double h);
  virtual void ThickTrack(const double vIn[], double vOut[], double h);

private :
  TRKQuadrupole(); ///< not implemented

};

#endif
