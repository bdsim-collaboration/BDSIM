#ifndef TRKDrift_h
#define TRKDrift_h

#include "TRKElement.hh"

/**
 * @brief drift
 */
class TRKDrift : public TRKElement {  
public:
  TRKDrift(std::string name, double length, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKDrift();
  
  virtual void Track(TRKBunch* bunch, TRKStrategy* strategy);
  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKDrift &element);

private:
  TRKDrift(); ///< not implemented
};

#endif
