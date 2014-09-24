#ifndef TRKQuadrupole_h
#define TRKQuadrupole_h

#include "TRKElement.hh"

/**
 * @brief quadrupole
 */
class TRKQuadrupole : public TRKElement {
protected:
  /// strength in Tesla/meter
  double strength;
  
public:
  TRKQuadrupole(double strength, std::string name, double length, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKQuadrupole();
  
  double GetStrength()const{return strength;}

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKQuadrupole &element);

private:
  TRKQuadrupole(); ///< not implemented
};

#endif
