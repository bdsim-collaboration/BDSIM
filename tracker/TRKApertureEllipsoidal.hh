#ifndef TRKApertureEllipsoidal_h
#define TRKApertureEllipsoidal_h 1

#include "TRKAperture.hh"
#include "TRKParticle.hh"

/**
 * @brief Ellipsoidal aperture class
 */
class TRKApertureEllipsoidal: public TRKAperture {
public:
  TRKApertureEllipsoidal(double xWidthIn, double yWidthIn);
  virtual ~TRKApertureEllipsoidal();

  virtual bool     OutsideAperture(const TRKParticle& particle);
  void             CheckAperture  (TRKBunch* bunch);

private:
  double xWidth;
  double yWidth;
  //note these are specified separately so the subsequent
  //variables can be set in the initialiser list by
  //performing calculations on these variables
  double xWidthSq;
  double yWidthSq;
  double xyWidthSqProd;
  TRKApertureEllipsoidal(); ///< not implemented
};

#endif
