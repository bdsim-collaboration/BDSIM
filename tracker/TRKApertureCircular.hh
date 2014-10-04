#ifndef TRKApertureCircular_h
#define TRKApertureCircular_h 1

#include "TRKAperture.hh"

class TRKParticle;
class TRKBunch;

/**
 * @brief Circular aperture class
 */
class TRKApertureCircular: public TRKAperture {
  //friend class TRKAperture;
public:
  TRKApertureCircular(double radius);
  virtual ~TRKApertureCircular();

  virtual bool OutsideAperture(const TRKParticle& particle);
  void         CheckAperture  (TRKBunch* bunch);

private:
  double radius;
  double radiusSq;
  TRKApertureCircular(); ///< not implemented
};

#endif
