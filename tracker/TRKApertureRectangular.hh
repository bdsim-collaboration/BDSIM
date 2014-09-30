#ifndef TRKApertureRectangular_h
#define TRKApertureRectangular_h 1

#include "TRKAperture.hh"
#include "TRKParticle.hh"

/**
 * @brief Rectangular aperture class
 */
class TRKApertureRectangular: public TRKAperture {
public:
  TRKApertureRectangular(double xWidthIn, double yWidthIn);
  virtual ~TRKApertureRectangular();

  virtual bool OutsideAperture(const TRKParticle& particle);

private:
  double xWidth;
  double yWidth;
  TRKApertureRectangular(); ///< not implemented
};

#endif
