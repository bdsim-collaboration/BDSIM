#include "BDSTiltOffset.hh"

BDSTiltOffset::BDSTiltOffset():
  dx(0.0), dy(0.0), tilt(0.0)
{;}

BDSTiltOffset::BDSTiltOffset(G4double xOffset,
			     G4double yOffset,
			     G4double tiltIn):
  dx(xOffset), dy(yOffset), tilt(tiltIn)
{;}

std::ostream& operator<< (std::ostream& out, BDSTiltOffset const &to)
{
  out << to.tilt << " " << to.dx << " " << to.dy;

  return out;
}
