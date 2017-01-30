#ifndef TRKQuadrupole_h
#define TRKQuadrupole_h

#include "TRKElement.hh"

/**
 * @brief Quadrupole. Strength in Tesla/metre.
 */

class TRKQuadrupole: public TRKElement
{
public:
  TRKQuadrupole(double        strength,
		std::string   name,
		double        length,
		TRKAperture  *aperture,
		TRKPlacement *placement);
  virtual ~TRKQuadrupole();

  virtual void Track(TRKBunch* bunch, TRKStrategy* strategy);

protected:
  /// output stream
  virtual void Print(std::ostream& out) const;

private:
  TRKQuadrupole(); ///< not implemented
};

#endif
