#include "BDSTiltOffset.hh"
#include "BDSUtilities.hh"

#include <ostream>

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

G4bool BDSTiltOffset::HasFiniteOffset() const
{
  return BDS::IsFinite(dx) || BDS::IsFinite(dy);
}

G4bool BDSTiltOffset::HasFiniteTilt() const
{
  return BDS::IsFinite(tilt);
}
