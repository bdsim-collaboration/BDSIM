#ifndef TRKAperture_h
#define TRKAperture_h 1

#include "TRK.hh"

#include "TRKParticle.hh"
#include "TRKBunch.hh"

#include <ostream>

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

  virtual std::ostream& PrintDetails(std::ostream& out) const = 0;
  //you can't make a friend operator virtual so wrap the real output in a method 
  //and use that polymorphically
  friend std::ostream& operator<< (std::ostream& out, const TRKAperture& aperture);
};

#endif
