#ifndef TRKSextupole_h
#define TRKSextupole_h

#include "TRKElement.hh"

/**
 * @brief Sextupole
 */
class TRKSextupole : public TRKElement {
private:
  /// Strength in Tesla/meter^2
  double strength;
  
public:
  TRKSextupole(double strength, std::string name, double length, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKSextupole();

  double GetStrength()const{return strength;}

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKSextupole &element);

private:
  TRKSextupole(); ///< not implemented
};

#endif
