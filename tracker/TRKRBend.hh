#ifndef TRKRBend_h
#define TRKRBend_h

#include "TRKElement.hh"

/**
 * @brief bend
 * 
 * dipole tracking but reference s-position is changed
 */
class TRKRBend : public TRKElement {
private: 
  /// bending angle in rad
  double angle;

public:
  TRKRBend(double angle, std::string name, double length, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKRBend();
  
  double GetAngle()const{return angle;}
  
  virtual void Track(TRKBunch* bunch, TRKStrategy* strategy);
  
protected:
  /// output stream
  virtual void Print(std::ostream& out) const;

private:
  TRKRBend(); ///< not implemented
};

#endif
