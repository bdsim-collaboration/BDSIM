#ifndef TRKApertureCircular_h
#define TRKApertureCircular_h 1

#include "TRKAperture.hh"
#include "TRKParticle.hh"
#include "TRKBunch.hh"
/**
 * @brief Circular aperture class
 */
class TRKApertureCircular: public TRKAperture {
public:
  TRKApertureCircular(double radius);
  virtual ~TRKApertureCircular();

  virtual bool OutsideAperture(const TRKParticle& particle);
  //virtual void CheckBunch(TRKBunch& bunch);

private:
  double radius;
  double radiusSq;
  TRKApertureCircular(); ///< not implemented
};

#endif
