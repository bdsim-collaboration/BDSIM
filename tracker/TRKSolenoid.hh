#ifndef TRKSolenoid_h
#define TRKSolenoid_h

#include "TRKTrackingElement.hh"
#include "TRKDrift.hh"

class vector6;

/**
 * @brief solenoid tracking
 */
class TRKSolenoid : public TRKTrackingElement {
private: 
  /// strength in Tesla/meter
  double strength;
  
public :
  TRKSolenoid(double strength, TRKTrackingElement::TRKType type, int steps, std::string name, double length, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKSolenoid();
  
  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKSolenoid &element);

protected:
  virtual void ThinTrack(const double vIn[], double vOut[], double h);
  virtual void HybridTrack(const double vIn[], double vOut[], double h);
  virtual void ThickTrack(const double vIn[], double vOut[], double h);

private:
  TRKSolenoid(); ///< not implemented
  TRKDrift* drift; // Drifts required for thin elements and zero strength
};

#endif
