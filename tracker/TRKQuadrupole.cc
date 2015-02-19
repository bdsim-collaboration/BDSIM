#include "TRKQuadrupole.hh"
#include "TRKStrategy.hh"
#include "TRKTiltOffset.hh"
#include "TRKBunch.hh"

TRKQuadrupole::TRKQuadrupole(double strengthIn, std::string nameIn, double lengthIn, TRKAperture *apertureIn, TRKPlacement *placementIn):
  TRKElement(nameIn,lengthIn,apertureIn,placementIn), strength(strengthIn)
{
}

TRKQuadrupole::~TRKQuadrupole() {
}

void TRKQuadrupole::Track(TRKBunch* bunch, TRKStrategy* strategy) {
#ifdef TRKDEBUG
  std::cout << "Tracking through element named: " << name << std::endl;
  std::cout << "before tracking" << std::endl;
  std::cout << *bunch;
#endif

  if (offsetIn && offsetOut) strategy->Track(offsetIn,bunch);

  strategy->Track(this,bunch);
  
  if (offsetIn && offsetOut) strategy->Track(offsetOut,bunch);

#ifdef TRKDEBUG
  std::cout << "after tracking" << std::endl;
  std::cout << *bunch;
#endif
}

void TRKQuadrupole::Print(std::ostream &out)const {
  TRKElement::Print(out);
  out << "; Strength: " << strength << " T/m";
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
