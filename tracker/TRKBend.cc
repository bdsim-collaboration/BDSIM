#include "TRKBend.hh"

#include <cmath>
#include <cstdlib>
#include "vector3.hh"
#include "vector6.hh"

TRKBend::TRKBend(double strengthIn, double angleIn, TRKTrackingElement::TRKType typeIn, int trackingStepsIn, std::string nameIn, double lengthIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKTrackingElement(typeIn, trackingStepsIn,nameIn,lengthIn,apertureIn,placementIn), strength(strengthIn), angle(angleIn), drift(NULL)
{
  drift = new TRKDrift(typeIn, trackingStepsIn,nameIn+"Drift",lengthIn,apertureIn,placementIn);
}

TRKBend::~TRKBend() {
  delete drift;
}

void TRKBend::ThinTrack(const double /*vIn*/[], double /*vOut*/[], double /*h*/) {  

}

void TRKBend::ThinKick(const vector6& /*vIn*/, vector6& /*vOut*/) {

}

void TRKBend::HybridTrack(const double /*vIn*/[], double /*vOut*/[], double /*h*/) {  

}

void TRKBend::ThickTrack(const double /*vIn*/[], double /*vOut*/[], double /*h*/) {

}

std::ostream& operator<< (std::ostream &out, const TRKBend &element) {
  return out << element.strength << " " << element.angle << " ";
}
