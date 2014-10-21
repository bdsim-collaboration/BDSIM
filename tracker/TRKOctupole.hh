#ifndef TRKOctupole_h
#define TRKOctupole_h

#include "TRKElement.hh"

/**
 * @brief Octupole
 */
class TRKOctupole : public TRKElement {
private:
  /// Strength in Tesla/meter^3
  double strength;
  
public:
  TRKOctupole(double strength, std::string name, double length, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKOctupole();

  virtual void Track(TRKBunch* bunch, TRKStrategy* strategy);

  double GetStrength()const{return strength;}

protected:
  /// output stream
  virtual void Print(std::ostream& out) const;

private:
  TRKOctupole(); ///< not implemented
};

#endif
