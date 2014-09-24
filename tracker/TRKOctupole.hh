#ifndef TRKOctupole_h
#define TRKOctupole_h

#include "TRKElement.hh"

/**
 * @brief sextupole tracking
 */
class TRKOctupole : public TRKElement {
private:
  /// Strength in Tesla/meter^3
  double strength;
  
public:
  TRKOctupole(double strength, std::string name, double length, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKOctupole();

  double GetStrength()const{return strength;}

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKOctupole &element);

private:
  TRKOctupole(); ///< not implemented
};

#endif
