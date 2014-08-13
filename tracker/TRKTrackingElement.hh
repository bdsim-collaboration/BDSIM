#ifndef TRKTrackingElement_h
#define TRKTrackingElement_h

#include "TRK.h"
#include "TRKAperture.hh"
#include "TRKElement.hh"


/**
 * @brief virtual base class for an element that can be tracked
 */
class TRKTrackingElement : public TRKElement {
public : 
  /* tracking type
   * thin: thin lens tracking
   * thick: thick lens tracking
   * hybrid: Geant4 field stepper
   */
  enum TRKType {thin, thick, hybrid};
  TRKTrackingElement(TRKType type, int trackingSteps, TRKElement &e);
  TRKTrackingElement(TRKType type, int trackingSteps, 
		     std::string name, double length, 
		     double size_x, double size_y, TRKAperture *aperture, TRKPlacement *placement);
		     
  virtual ~TRKTrackingElement(); 

  /**
   * Tracks a 6-dim vector
   * @param[out] vOut The 6 dimensional output vector.
   * @param[in]  vIn  The 6 dimensional input vector.
   * @param[in]  h    The step length in mm.
   */
  virtual void Track(const double vIn[], double vOut[], double h);
  virtual void Track(const double vIn[], double vOut[]);

  /// returns tracking type
  TRKType trackingType()const {return type;}

protected: 
  /// type of tracking 
  virtual void ThinTrack(const double vIn[], double vOut[], double h) = 0;
  virtual void HybridTrack(const double vIn[], double vOut[], double h) = 0;
  virtual void ThickTrack(const double vIn[], double vOut[], double h) = 0;

  /// tracking type
  TRKType type;
  /// number of tracking steps
  int trackingSteps;

private:
  TRKTrackingElement(); ///< not implemented
};


#endif
