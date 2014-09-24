#ifndef TRKBend_h
#define TRKBend_h

#include "TRKElement.hh"

/**
 * @brief bend
 */
class TRKBend : public TRKElement {
private: 
  /// strength in Tesla/meter
  double strength;
  /// bending angle in rad
  double angle;

public:
  TRKBend(double strength, double angle, std::string name, double length, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKBend();
  
  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKBend &element);

private:
  TRKBend(); ///< not implemented
};

#endif
