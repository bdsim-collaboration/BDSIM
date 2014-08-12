#ifndef TRKTrackingInterface_h
#define TRKTrackingInterface_h

class TRKTrackingInterface {
public : 
  enum TRKtype {thin, thick, hybrid};
  TRKTrackingInterface();
  virtual ~TRKTrackingInterface(); 

  /**
   * Tracks a 6-dim vector
   * @param[out] vOut The 6 dimensional output vector.
   * @param[in]  vIn  The 6 dimensional input vector.
   * @param[in]  h    The step length relative to the full length of the element
  */

  virtual void Track(const double vIn[], double vOut[], double h) = 0;
  virtual void Track(const double vIn[], double vOut[]) = 0;

protected: 

  TRKtype type;

  // type of tracking 
  virtual void ThinTrack(const double vIn[], double vOut[], double h) = 0;
  virtual void HybridTrack(const double vIn[], double vOut[], double h) = 0;
  virtual void ThickTrack(const double vIn[], double vOut[], double h) = 0;
  int trackingSteps;

};


#endif
