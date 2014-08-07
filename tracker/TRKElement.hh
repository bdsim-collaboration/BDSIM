#ifndef TRKElement_h
#define TRKElement_h

#include <string>

#include "TRKTrackingInterface.hh"
#include "TRKApertureInterface.hh"
#include "TRKPlacement.hh"

class TRKElement : public TRKTrackingInterface { 
 private: 
  std::string name;               // name of element
  double length;                  // length of component 
  double size_y;                  // height of component 
  double size_x;                  // width  of component 
  TRKApertureInterface *aperture; // aperture of element
  TRKPlacement         *placement;// location of element

public :
  TRKElement();
  TRKElement(double length, double size_x, double size_y);
  ~TRKElement();
};

#endif
