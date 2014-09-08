#ifndef TRKSextupole_h
#define TRKSextupole_h

#include "TRKTrackingElement.hh"
#include "TRKDrift.hh"

class vector6;

/**
 * @brief sextupole tracking
 */
class TRKSextupole : public TRKTrackingElement {
private:
  /// Strength in Tesla/meter^2
  double strength;
  
public:
  TRKSextupole(double strength, TRKTrackingElement::TRKType type, int steps, std::string name, double length, double size_x, double size_y, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKSextupole();
  
  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKSextupole &element);

protected:
  virtual void ThinTrack(const double vIn[], double vOut[], double h);
  virtual void HybridTrack(const double vIn[], double vOut[], double h);
  virtual void ThickTrack(const double vIn[], double vOut[], double h);

private:
  TRKSextupole(); ///< not implemented
  TRKDrift* drift; // Drifts required for thin elements and zero strength

  /// thin Kick method
  void ThinKick(const vector6& vIn, vector6& vOut);
};

#endif
