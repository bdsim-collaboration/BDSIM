#ifndef TRKDipole_h
#define TRKDipole_h

#include "TRKElement.hh"

/**
 * @brief dipole/kicker. Strength in Tesla/metre.
 */

class TRKDipole: public TRKElement
{
public:
  TRKDipole(double      strength,
	    double      bField,
	    std::string name,
	    double      length,
	    TRKAperture *aperture,
	    TRKPlacement *placement);
  virtual ~TRKDipole();

  virtual void Track(TRKBunch* bunch, TRKStrategy* strategy);  
  
  double GetIntStrength()const{return intStrength;}

protected:
  /// output stream
  virtual void Print(std::ostream& out) const;

private:
  TRKDipole(); ///< not implemented

  /// magnetic field in Tesla - only used for hybrid tracking
  double bField;
  /// integrated strength MeV * urad
  double intStrength;
};

#endif
