#ifndef TRKDrift_h
#define TRKDrift_h

#include <string>

#include "vector3.hh"
#include "TRKTrackingElement.hh"

/**
 * @brief drift tracking
 */
class TRKDrift : virtual public TRKTrackingElement { 
public :
  TRKDrift(std::string name, double length, double size_x, double size_y, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKDrift(); 

protected:
  // Specific implementations of different tracking routines 
  virtual void ThinTrack(const double vIn[], double vOut[], double h);
  virtual void HybridTrack(const double vIn[], double vOut[], double h);
  virtual void ThickTrack(const double vIn[], double vOut[], double h);
  
private : 
  TRKDrift(); ///< not implemented
    
};

#endif
