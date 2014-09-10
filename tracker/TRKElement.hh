#ifndef TRKElement_h
#define TRKElement_h

#include <string>
#include <iostream>

#include "TRKAperture.hh"
#include "TRKPlacement.hh"

/**
 * @brief a basic element
 */
class TRKElement { 
public :
  TRKElement(std::string name, double length, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKElement();

  std::string GetName()const {return name;}

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKElement &element);

protected : 
  std::string  name;               ///< name of element
  double       length;             ///< length of component [m]
  TRKAperture  *aperture;          ///< aperture of element
  TRKPlacement *placement;         ///< location of element

private :
  TRKElement(); ///< not implemented

};

#endif
