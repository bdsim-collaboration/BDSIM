#ifndef TRKAperture_h
#define TRKAperture_h

#include "TRKParticle.hh"
/**
 * @brief Aperture class
 */

class TRKAperture {
public : 
  /** aperture shapes
   * ellipsoid: circular shaped beampipe
   * rectangle: rectangular shaped beampipe
   */
  enum TRKShape {ellipsoid, rectangle};

  TRKAperture(double size_x, double size_y, TRKShape shape);
  ~TRKAperture(); 

  /// checks if aperture is hit
  bool CheckAperture(const TRKParticle& /*particle*/){
    return false; // for now
  }
  
private :
  double       sizeX;   ///< width  of component [m]
  double       sizeY;   ///< height of component [m]

  TRKShape     shape;    ///< shape of aperture

  TRKAperture(); ///< not implemented

};


#endif 
