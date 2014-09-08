#include "TRKDecapole.hh"

#include <cmath>
#include <cstdlib>
#include "vector3.hh"
#include "vector6.hh"

TRKDecapole::TRKDecapole(double strengthIn, TRKTrackingElement::TRKType typeIn, int trackingStepsIn, std::string nameIn, double lengthIn, double size_xIn, double size_yIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKTrackingElement(typeIn, trackingStepsIn,nameIn,lengthIn,size_xIn,size_yIn,apertureIn,placementIn), strength(strengthIn), drift(NULL)
{
  drift = new TRKDrift(typeIn, trackingStepsIn,nameIn+"Drift",lengthIn,size_xIn,size_yIn,apertureIn,placementIn);
}

TRKDecapole::~TRKDecapole() {
  delete drift;
}

void TRKDecapole::ThinTrack(const double /*vIn*/[], double /*vOut*/[], double /*h*/) {  

}

void TRKDecapole::ThinKick(const vector6& /*vIn*/, vector6& /*vOut*/) {

}

void TRKDecapole::HybridTrack(const double /*vIn*/[], double /*vOut*/[], double /*h*/) {  

}

void TRKDecapole::ThickTrack(const double /*vIn*/[], double /*vOut*/[], double /*h*/) {

}

std::ostream& operator<< (std::ostream &out, const TRKDecapole &element) {
  return out << element.strength << " ";
}
