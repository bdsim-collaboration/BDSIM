#ifndef TRKSolenoid_h
#define TRKSolenoid_h

#include "TRKElement.hh"

/**
 * @brief Solenoid. Strength in Tesla/metre.
 */

class TRKSolenoid: public TRKElement
{
public:
  TRKSolenoid(double        strength,
	      std::string   name,
	      double        length,
	      TRKAperture  *aperture,
	      TRKPlacement *placement);
  virtual ~TRKSolenoid();

  virtual void Track(TRKBunch* bunch, TRKStrategy* strategy);
  
protected:
  /// output stream
  virtual void Print(std::ostream& out) const;

private:
  TRKSolenoid(); ///< not implemented
};

#endif
