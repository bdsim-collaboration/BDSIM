#ifndef TRKElement_h
#define TRKElement_h

#include <string>

#include "TRKAperture.hh"
#include "TRKPlacement.hh"

/**
 * @brief a basic element
 */
class TRKElement { 
public :
  TRKElement(std::string name, double length, double size_x, double size_y, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKElement();

protected : 
  std::string  name;               ///< name of element
  double       length;             ///< length of component [m]
  double       size_x;             ///< width  of component [m]
  double       size_y;             ///< height of component [m]
  TRKAperture  *aperture;          ///< aperture of element
  TRKPlacement *placement;         ///< location of element

private :
  TRKElement(); ///< not implemented

};

#endif
