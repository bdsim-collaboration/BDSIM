#ifndef TRKDecapole_h
#define TRKDecapole_h

#include "TRKElement.hh"

/**
 * @brief sextupole tracking
 */
class TRKDecapole : public TRKElement {
private:
  /// Strength in Tesla/meter^3
  double strength;
  
public:
  TRKDecapole(double strength, std::string name, double length, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKDecapole();

  virtual void Track(TRKBunch* bunch, TRKStrategy* strategy);
  
protected:
  /// output stream
  virtual void Print(std::ostream& out) const;

private:
  TRKDecapole(); ///< not implemented
};

#endif
