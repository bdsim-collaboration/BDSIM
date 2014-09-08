#include "TRKSextupole.hh"

#include <cmath>
#include <cstdlib>
#include "vector3.hh"
#include "vector6.hh"

TRKSextupole::TRKSextupole(double strengthIn, TRKTrackingElement::TRKType typeIn, int trackingStepsIn, std::string nameIn, double lengthIn, double size_xIn, double size_yIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKTrackingElement(typeIn, trackingStepsIn,nameIn,lengthIn,size_xIn,size_yIn,apertureIn,placementIn), strength(strengthIn), drift(NULL)
{
  drift = new TRKDrift(typeIn, trackingStepsIn,nameIn+"Drift",lengthIn,size_xIn,size_yIn,apertureIn,placementIn);
}

TRKSextupole::~TRKSextupole() {
  delete drift;
}

void TRKSextupole::ThinTrack(const double /*vIn*/[], double /*vOut*/[], double /*h*/) {  

}

void TRKSextupole::ThinKick(const vector6& /*vIn*/, vector6& /*vOut*/) {

}

void TRKSextupole::HybridTrack(const double /*vIn*/[], double /*vOut*/[], double /*h*/) {  

}

void TRKSextupole::ThickTrack(const double /*vIn*/[], double /*vOut*/[], double /*h*/) {

}

std::ostream& operator<< (std::ostream &out, const TRKSextupole &element) {
  return out << element.strength << " ";
}
