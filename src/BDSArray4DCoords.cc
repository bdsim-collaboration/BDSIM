#include "BDSArray4DCoords.hh"
#include "BDSFieldValue.hh"

#include <ostream>

#include "globals.hh"

BDSArray4DCoords::BDSArray4DCoords(G4int nXIn, G4int nYIn, G4int nZIn, G4int nTIn,
				   G4double xMinIn, G4double xMaxIn,
				   G4double yMinIn, G4double yMaxIn,
				   G4double zMinIn, G4double zMaxIn,
				   G4double tMinIn, G4double tMaxIn):
  BDSArray4D(nXIn,nYIn,nZIn,nTIn),
  xMin(xMinIn), xMax(xMaxIn),
  yMin(yMinIn), yMax(yMaxIn),
  zMin(zMinIn), zMax(zMaxIn),
  tMin(tMinIn), tMax(tMaxIn)
{
  xStep = (xMax - xMin) / ((G4double)nX - 1);
  yStep = (yMax - yMin) / ((G4double)nY - 1);
  zStep = (zMax - zMin) / ((G4double)nZ - 1);
  tStep = (tMax - tMin) / ((G4double)nT - 1);
}

G4bool BDSArray4DCoords::OutsideCoords(const G4double x,
				       const G4double y,
				       const G4double z,
				       const G4double t) const
{
  G4bool rx = x < xMin || x > xMax;
  G4bool ry = y < yMin || y > yMax;
  G4bool rz = z < zMin || z > zMax;
  G4bool rt = t < tMin || t > tMax;
  return rx || ry || rz || rt;
}

void BDSArray4DCoords::OutsideCoordsWarn(const G4double x,
					 const G4double y,
					 const G4double z,
					 const G4double t) const
{
  if (Outside(x,y,z,t))
    {
      G4cerr << "(" << x << ", " << y << ", " << z << ", " << t
	     << ") is outside array" << G4endl;
      exit(1);
    }
}

std::ostream& BDSArray4DCoords::Print(std::ostream& out) const
{
  out << "X: (" << xMin << ", " << xMax << ")" << G4endl;
  out << "Y: (" << yMin << ", " << yMax << ")" << G4endl;
  out << "Z: (" << zMin << ", " << zMax << ")" << G4endl;
  out << "T: (" << tMin << ", " << tMax << ")" << G4endl;
  return BDSArray4D::Print(out);
}

std::ostream& operator<< (std::ostream& out, BDSArray4DCoords const &a)
{
  return a.Print(out);
}
