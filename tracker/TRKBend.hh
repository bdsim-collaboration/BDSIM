#ifndef TRKBend_h
#define TRKBend_h

#include "TRKTrackingElement.hh"
#include "TRKDrift.hh"

class vector6;

/**
 * @brief bend tracking
 */
class TRKBend : public TRKTrackingElement {
private: 
  /// strength in Tesla/meter
  double strength;
  /// bending angle in rad
  double angle;

public:
  TRKBend(double strength, double angle, TRKTrackingElement::TRKType type, int steps, std::string name, double length, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKBend();
  
  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKBend &element);

protected:
  virtual void ThinTrack(const double vIn[], double vOut[], double h);
  virtual void HybridTrack(const double vIn[], double vOut[], double h);
  virtual void ThickTrack(const double vIn[], double vOut[], double h);

private:
  TRKBend(); ///< not implemented
  TRKDrift* drift; // Drifts required for thin elements and zero strength

  /// thin Kick method
  void ThinKick(const vector6& vIn, vector6& vOut);
};

#endif
