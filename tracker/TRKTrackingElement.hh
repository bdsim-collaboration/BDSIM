#ifndef TRKTrackingElement_h
#define TRKTrackingElement_h

#define DEFAULT_TRACKING_STEPS 10;

#include "TRKAperture.hh"
#include "TRKElement.hh"

class TRKTrackingElement : public TRKElement {
public : 
  enum TRKType {thin, thick, hybrid};
  TRKTrackingElement();
  TRKTrackingElement(TRKType type, int trackingSteps, TRKElement &e);
  TRKTrackingElement(TRKType type, int trackingSteps, 
		     std::string name, double length, 
		     double size_x, double size_y, TRKAperture *aperture, TRKPlacement *placement);
		     
  virtual ~TRKTrackingElement(); 

  /**
   * Tracks a 6-dim vector
   * @param[out] vOut The 6 dimensional output vector.
   * @param[in]  vIn  The 6 dimensional input vector.
   * @param[in]  h    The step length relative to the full length of the element
  */

  virtual void Track(const double vIn[], double vOut[], double h);
  virtual void Track(const double vIn[], double vOut[]);

  // type of tracking 
  virtual void ThinTrack(const double vIn[], double vOut[], double h) = 0;
  virtual void HybridTrack(const double vIn[], double vOut[], double h) = 0;
  virtual void ThickTrack(const double vIn[], double vOut[], double h) = 0;

  TRKType trackingType() {return type;}
      
protected: 

  TRKType type;
  int trackingSteps;

};


#endif
