#ifndef TRKAperture_h
#define TRKAperture_h 1

#include "TRK.hh"

#include "TRKParticle.hh"
#include "TRKBunch.hh"

/**
 * @brief Aperture base class
 */

class TRKAperture {
public : 
  TRKAperture(){};
  virtual ~TRKAperture(){}; 

  /// checks if aperture is hit - specific to each type of aperture
  //  the actual algorithm here and purely virtual
  virtual bool OutsideAperture(const TRKParticle& particle)=0;
  
  /// loop over bunch and apply aperture test - implemented in this base class
  void CheckAperture(TRKBunch* bunch);

  //Should also provide a << cout stream implementation for each and declare here
};

#endif
