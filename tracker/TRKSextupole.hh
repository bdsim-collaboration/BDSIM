#ifndef TRKSextupole_h
#define TRKSextupole_h

#include "TRKElement.hh"

/**
 * @brief Sextupole
 */

class TRKSextupole: public TRKElement
{
public:
  TRKSextupole(double       strength,
	       std::string  name,
	       double       length,
	       TRKAperture  *aperture,
	       TRKPlacement *placement);
  virtual ~TRKSextupole();

  virtual void Track(TRKBunch* bunch, TRKStrategy* strategy);

  inline double GetStrength() const {return strength;}

protected:
  /// output stream
  virtual void Print(std::ostream& out) const;

private:
  TRKSextupole(); ///< not implemented

  /// Strength in Tesla/meter^2
  double strength;
};

#endif
