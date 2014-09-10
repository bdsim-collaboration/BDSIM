#include "TRKSolenoid.hh"

#include <cmath>
#include <cstdlib>
#include "vector3.hh"
#include "vector6.hh"

TRKSolenoid::TRKSolenoid(double strengthIn, TRKTrackingElement::TRKType typeIn, int trackingStepsIn, std::string nameIn, double lengthIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKTrackingElement(typeIn, trackingStepsIn,nameIn,lengthIn,apertureIn,placementIn), strength(strengthIn), drift(NULL)
{
  drift = new TRKDrift(typeIn, trackingStepsIn,nameIn+"Drift",lengthIn,apertureIn,placementIn);
}

TRKSolenoid::~TRKSolenoid() {
  delete drift;
}

void TRKSolenoid::ThinTrack(const double vIn[], double vOut[], double h) {  
  if (std::abs(strength)<=1e-12) {
    return drift->Track(vIn,vOut,h);
  }

}

void TRKSolenoid::HybridTrack(const double vIn[], double vOut[], double h) {  
  if (std::abs(strength)<=1e-12) {
    return drift->Track(vIn,vOut,h);
  }
}

void TRKSolenoid::ThickTrack(const double vIn[], double vOut[], double h) {
  if (std::abs(strength)<=1e-12) {
    return drift->Track(vIn,vOut,h);
  }
}

std::ostream& operator<< (std::ostream &out, const TRKSolenoid &element) {
  return out << element.strength << " ";
}
