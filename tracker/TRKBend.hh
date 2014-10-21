#ifndef TRKBend_h
#define TRKBend_h

#include "TRKElement.hh"

/**
 * @brief bend
 * 
 * dipole tracking but reference s-position is changed
 */
class TRKBend : public TRKElement {
private: 
  /// strength in Tesla/meter
  double strength;
  /// bending angle in rad
  double angle;

public:
  TRKBend(double strength, double angle, std::string name, double length, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKBend();
  
  virtual void Track(TRKBunch* bunch, TRKStrategy* strategy);
  
protected:
  /// output stream
  virtual void Print(std::ostream& out) const;

private:
  TRKBend(); ///< not implemented
};

#endif
