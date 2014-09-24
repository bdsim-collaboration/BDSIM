#ifndef TRKDecapole_h
#define TRKDecapole_h

#include "TRKElement.hh"

/**
 * @brief sextupole tracking
 */
class TRKDecapole : public TRKElement {
private:
  /// Strength in Tesla/meter^3
  double strength;
  
public:
  TRKDecapole(double strength, std::string name, double length, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKDecapole();
  
  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKDecapole &element);

private:
  TRKDecapole(); ///< not implemented
};

#endif
