#ifndef TRKRBend_h
#define TRKRBend_h

#include "TRKElement.hh"

/**
 * @brief bend
 * 
 * dipole tracking but reference s-position is changed
 */

class TRKRBend: public TRKElement
{
public:
  TRKRBend(double        angle,
	   std::string   name,
	   double        length,
	   TRKAperture  *aperture,
	   TRKPlacement *placement);
  virtual ~TRKRBend();
  
  inline double GetAngle() const {return angle;}
  
  virtual void Track(TRKBunch* bunch, TRKStrategy* strategy);
  
protected:
  /// output stream
  virtual void Print(std::ostream& out) const;

private:
  TRKRBend(); ///< not implemented

  /// bending angle in rad
  double angle;
};

#endif
