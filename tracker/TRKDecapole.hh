#ifndef TRKDecapole_h
#define TRKDecapole_h

#include "TRKElement.hh"

/**
 * @brief sextupole tracking
 */

class TRKDecapole: public TRKElement
{ 
public:
  TRKDecapole(double        strength,
	      std::string   name,
	      double        length,
	      TRKAperture  *aperture,
	      TRKPlacement *placement);
  virtual ~TRKDecapole();

  virtual void Track(TRKBunch* bunch, TRKStrategy* strategy);

  inline double GetStrength() const {return strength;}
  
protected:
  /// output stream
  virtual void Print(std::ostream& out) const;

private:
  TRKDecapole(); ///< not implemented

  /// Strength in Tesla/meter^3
  double strength;
};

#endif
