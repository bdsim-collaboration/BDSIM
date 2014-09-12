#ifndef TRKTrackingElement_h
#define TRKTrackingElement_h

#include "TRK.hh"
#include "TRKAperture.hh"
#include "TRKElement.hh"

/**
 * @brief virtual base class for an element that can be tracked
 */
class TRKTrackingElement : public TRKElement {
public : 
  /** tracking type
   * thin: thin lens tracking
   * thin 
   * thick: thick lens tracking
   * hybrid: Geant4 field stepper
   */
  enum TRKType {thin, thinsymplectic, thick, hybrid};
  TRKTrackingElement(TRKType type, int trackingSteps, TRKElement &e);
  TRKTrackingElement(TRKType type, int trackingSteps, 
		     std::string name, double length, 
		     TRKAperture *aperture, TRKPlacement *placement);
		     
  virtual ~TRKTrackingElement(); 

  /// returns tracking type
  TRKType trackingType()const {return type;}

  /**
   * Tracks a 6-dim vector
   * @param[out] vOut The 6 dimensional output vector.
   * @param[in]  vIn  The 6 dimensional input vector.
   * @param[in]  h    The step length in mm.
   */
  ///@{
  void Track(const double vIn[], double vOut[]);
  void Track(const double vIn[], double vOut[], double h);
  ///@}

protected: 
  // type of tracking 
  /// thin lens kick tracking
  virtual void ThinTrack(const double vIn[], double vOut[], double h) = 0;
  /// thin lens kick tracking symplectic
  /// defaults to ThinTrack, can be overwritten by derived classes
  virtual void ThinTrackSymplectic(const double vIn[], double vOut[], double h){
    ThinTrack(vIn, vOut, h);
  }
  /// bdsim ("hybrid") tracking
  virtual void HybridTrack(const double vIn[], double vOut[], double h) = 0;
  /// thick lens tracking
  virtual void ThickTrack(const double vIn[], double vOut[], double h) = 0;

  /// tracking type
  TRKType type;

private:
  TRKTrackingElement(); ///< not implemented

  /// store direction and location at beginning of tracking step , global coordinates
  void StoreParticle(const double vIn[])const;

  /// number of tracking steps
  int trackingSteps;

};


#endif
