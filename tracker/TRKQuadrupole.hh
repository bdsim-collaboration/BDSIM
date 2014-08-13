#ifndef TRKQuadrupole_h
#define TRKQuadrupole_h

#include "TRKTrackingElement.hh"

/**
 * @brief quadrupole tracking
 */
class TRKQuadrupole : public TRKTrackingElement {
private: 
  /// strength in Tesla/meter
  double strength;

public :   
  TRKQuadrupole(double strength, TRKTrackingElement::TRKType type, int steps, std::string name, double length, double size_x, double size_y, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKQuadrupole();
  
protected:
  virtual void ThinTrack(const double vIn[], double vOut[], double h);
  virtual void HybridTrack(const double vIn[], double vOut[], double h);
  virtual void ThickTrack(const double vIn[], double vOut[], double h);

private :
  TRKQuadrupole(); ///< not implemented

};

#endif
