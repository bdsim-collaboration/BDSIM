#ifndef TRKQuadrupole_h
#define TRKQuadrupole_h

#include "TRKElement.hh"

/**
 * @brief quadrupole
 */
class TRKQuadrupole : public TRKElement {
protected:
  /// strength in Tesla/meter
  double strength;
  
public:
  TRKQuadrupole(double strength, std::string name, double length, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKQuadrupole();
  
  double GetStrength()const{return strength;}

  virtual void Track(TRKBunch* bunch, TRKStrategy* strategy);

protected:
  /// output stream
  virtual void Print(std::ostream& out) const;

private:
  TRKQuadrupole(); ///< not implemented
};

#endif
