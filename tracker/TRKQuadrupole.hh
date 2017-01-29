#ifndef TRKQuadrupole_h
#define TRKQuadrupole_h

#include "TRKElement.hh"

/**
 * @brief quadrupole
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
  
  inline double GetStrength() const {return strength;}

  virtual void Track(TRKBunch* bunch, TRKStrategy* strategy);

protected:
  /// output stream
  virtual void Print(std::ostream& out) const;

  /// strength in Tesla/meter
  double strength;

private:
  TRKQuadrupole(); ///< not implemented
};

#endif
