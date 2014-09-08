#ifndef TRKOctopole_h
#define TRKOctopole_h

#include "TRKTrackingElement.hh"
#include "TRKDrift.hh"

class vector6;

/**
 * @brief sextupole tracking
 */
class TRKOctopole : public TRKTrackingElement {
private:
  /// Strength in Tesla/meter^3
  double strength;
  
public:
  TRKOctopole(double strength, TRKTrackingElement::TRKType type, int steps, std::string name, double length, double size_x, double size_y, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKOctopole();
  
  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKOctopole &element);

protected:
  virtual void ThinTrack(const double vIn[], double vOut[], double h);
  virtual void HybridTrack(const double vIn[], double vOut[], double h);
  virtual void ThickTrack(const double vIn[], double vOut[], double h);

private:
  TRKOctopole(); ///< not implemented
  TRKDrift* drift; // Drifts required for thin elements and zero strength

  /// thin Kick method
  void ThinKick(const vector6& vIn, vector6& vOut);
};

#endif
