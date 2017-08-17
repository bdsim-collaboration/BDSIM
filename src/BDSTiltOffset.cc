#include "BDSTiltOffset.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

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

G4Transform3D BDSTiltOffset::Transform3D() const
{
  G4ThreeVector off = GetOffset();
  G4RotationMatrix rm = G4RotationMatrix();
  rm.rotateZ(-tilt);
  return G4Transform3D(rm, off);
}

G4bool BDSTiltOffset::HasFiniteOffset() const
{
  return BDS::IsFinite(dx) || BDS::IsFinite(dy);
}

G4bool BDSTiltOffset::HasFiniteTilt() const
{
  return BDS::IsFinite(tilt);
}
