#ifndef TRKDrift_h
#define TRKDrift_h

#include <string>

#include "vector3.hh"
#include "TRKTrackingElement.hh"

/**
 * @brief drift tracking
 */
class TRKDrift : public TRKTrackingElement { 
public :
  //  TRKDrift();
  TRKDrift(TRKTrackingElement::TRKType, int trackingSteps, 
	   std::string name, double length, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKDrift(); 

protected:
  // Specific implementations of different tracking routines 
  void ThinTrack(const double vIn[], double vOut[], double h);
  void HybridTrack(const double vIn[], double vOut[], double h);
  void ThickTrack(const double vIn[], double vOut[], double h);
  
private : 
  TRKDrift(); ///< not implemented
    
};

#endif
