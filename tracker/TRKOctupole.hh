#ifndef TRKOctupole_h
#define TRKOctupole_h

#include "TRKElement.hh"

/**
 * @brief Octupole
 */

class TRKOctupole: public TRKElement
{  
public:
  TRKOctupole(double        strength,
	      std::string   name,
	      double        length,
	      TRKAperture  *aperture,
	      TRKPlacement *placement);
  virtual ~TRKOctupole();

  virtual void Track(TRKBunch* bunch, TRKStrategy* strategy);

  inline double GetStrength() const {return strength;}

protected:
  /// output stream
  virtual void Print(std::ostream& out) const;

private:
  TRKOctupole(); ///< not implemented

  /// Strength in Tesla/meter^3
  double strength;
};

#endif
