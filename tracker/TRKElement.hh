#ifndef TRKElement_h
#define TRKElement_h

class TRKElement { 
 private: 
  double length;                  // length of component 
  double size_y;                  // height of component 
  double size_x;                  // width  of component 
  TRKApertureInterface *aperture; // aperture of element

public :
  TRKElement();
  TRKElement(double length, double size_x, double size_y);
  ~TRKEement();
}

#endif
