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
  /// integrated strength MeV * urad
  double intStrength;

public:
  TRKDipole(double strength, double bField, std::string name, double length, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKDipole();

  virtual void Track(TRKBunch* bunch, TRKStrategy* strategy);  

  double GetStrength()const{return strength;}
  double GetIntStrength()const{return intStrength;}

protected:
  /// output stream
  virtual void Print(std::ostream& out) const;

private:
  TRKDipole(); ///< not implemented
};

#endif
