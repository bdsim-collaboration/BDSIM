#ifndef TRKSextupole_h
#define TRKSextupole_h

#include "TRKElement.hh"

/**
 * @brief Sextupole. Strength in Tesla/meter^2.
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

protected:
  /// output stream
  virtual void Print(std::ostream& out) const;

private:
  TRKSextupole(); ///< not implemented
};

#endif
