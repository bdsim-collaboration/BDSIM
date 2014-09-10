#include "TRKDipole.hh"

#include <cmath>
#include <cstdlib>
#include "vector3.hh"
#include "vector6.hh"

TRKDipole::TRKDipole(double strengthIn, TRKTrackingElement::TRKType typeIn, int trackingStepsIn, std::string nameIn, double lengthIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKTrackingElement(typeIn, trackingStepsIn,nameIn,lengthIn,apertureIn,placementIn), strength(strengthIn), drift(NULL)
{
  drift = new TRKDrift(typeIn, trackingStepsIn,nameIn+"Drift",lengthIn,apertureIn,placementIn);
}

TRKDipole::~TRKDipole() {
  delete drift;
}

void TRKDipole::ThinTrack(const double /*vIn*/[], double /*vOut*/[], double /*h*/) {  

}

void TRKDipole::ThinKick(const vector6& /*vIn*/, vector6& /*vOut*/) {

}

void TRKDipole::HybridTrack(const double /*vIn*/[], double /*vOut*/[], double /*h*/) {  

}

void TRKDipole::ThickTrack(const double /*vIn*/[], double /*vOut*/[], double /*h*/) {

}

std::ostream& operator<< (std::ostream &out, const TRKDipole &element) {
  return out << element.strength << " ";
}
