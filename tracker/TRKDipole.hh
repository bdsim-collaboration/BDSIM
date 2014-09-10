#ifndef TRKDipole_h
#define TRKDipole_h

#include "TRKTrackingElement.hh"
#include "TRKDrift.hh"

class vector6;

/**
 * @brief dipole/kicker tracking
 */
class TRKDipole : public TRKTrackingElement {
private: 
  /// strength in Tesla/meter
  double strength;
  
public:
  TRKDipole(double strength, TRKTrackingElement::TRKType type, int steps, std::string name, double length, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKDipole();
  
  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKDipole &element);

protected:
  virtual void ThinTrack(const double vIn[], double vOut[], double h);
  virtual void HybridTrack(const double vIn[], double vOut[], double h);
  virtual void ThickTrack(const double vIn[], double vOut[], double h);

private:
  TRKDipole(); ///< not implemented
  TRKDrift* drift; // Drifts required for thin elements and zero strength

  /// thin Kick method
  void ThinKick(const vector6& vIn, vector6& vOut);
};

#endif
