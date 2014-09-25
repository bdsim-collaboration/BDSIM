#ifndef TRKDipole_h
#define TRKDipole_h

#include "TRKElement.hh"

/**
 * @brief dipole/kicker
 */
class TRKDipole : public TRKElement {
private: 
  /// strength in Tesla/meter
  double strength;
  /// magnetic field in Tesla - only used for hybrid tracking
  double bField;

public:
  TRKDipole(double strength, double bField, std::string name, double length, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKDipole();
  
  double GetStrength()const{return strength;}

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKDipole &element);

private:
  TRKDipole(); ///< not implemented
};

#endif
