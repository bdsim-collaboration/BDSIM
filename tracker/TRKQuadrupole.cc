#include "TRKQuadrupole.hh"


TRKQuadrupole::TRKQuadrupole(double strengthIn, std::string nameIn, double lengthIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKElement(nameIn,lengthIn,apertureIn,placementIn), strength(strengthIn)
{
}

TRKQuadrupole::~TRKQuadrupole() {
}

std::ostream& operator<< (std::ostream &out, const TRKQuadrupole &element) {
  return out << element.strength << " ";
}

// void TRKQuadrupole::ThinKick(const vector6& vIn, vector6& vOut) {

//   vOut = vIn;

//   // double charge, oh charge good point, should just add this to method signature
//   double charge = 1 * TRK::e;
//   double rigidity = std::abs(strength) * vIn.mom().mag() / charge; // to be checked

//   double k = 1/std::sqrt(std::abs(rigidity));

//   if (k>0) {
//     vOut.plusmom(vector3(-vIn.X()*k*std::sin(k*length), vIn.Y()*k*std::sinh(k*length), 0.0));
//   } else {
//     vOut.plusmom(vector3(vIn.X()*k*std::sinh(k*length), -vIn.Y()*k*std::sin(k*length), 0.0));
//   }
// }
