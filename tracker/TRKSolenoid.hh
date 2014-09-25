#ifndef TRKSolenoid_h
#define TRKSolenoid_h

#include "TRKElement.hh"

/**
 * @brief Solenoid
 */
class TRKSolenoid : public TRKElement {
private: 
  /// strength in Tesla/meter
  double strength;
  
public :
  TRKSolenoid(double strength, std::string name, double length, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKSolenoid();
  
  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKSolenoid &element);

private:
  TRKSolenoid(); ///< not implemented
};

#endif
