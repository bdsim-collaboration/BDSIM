#include "BDSArray4DCoords.hh"
#include "BDSFieldValue.hh"

#include <ostream>

#include "globals.hh"

BDSArray4DCoords::BDSArray4DCoords(G4int nX, G4int nY, G4int nZ, G4int nT,
				   G4double xMinIn, G4double xMaxIn,
				   G4double yMinIn, G4double yMaxIn,
				   G4double zMinIn, G4double zMaxIn,
				   G4double tMinIn, G4double tMaxIn):
  BDSArray4D(nX,nY,nZ,nT),
  xMin(xMinIn), xMax(xMaxIn),
  yMin(yMinIn), yMax(yMaxIn),
  zMin(zMinIn), zMax(zMaxIn),
  tMin(tMinIn), tMax(tMaxIn)
{
  xStep = (xMax - xMin) / (G4double)nY;
  yStep = (yMax - yMin) / (G4double)nY;
  zStep = (zMax - zMin) / (G4double)nZ;
  tStep = (tMax - tMin) / (G4double)nT;
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

std::ostream& operator<< (std::ostream& out, BDSArray4DCoords const &a)
{
  out << "X: (" << a.xMin << ", " << a.xMax << ")" << G4endl;
  out << "Y: (" << a.yMin << ", " << a.yMax << ")" << G4endl;
  out << "Z: (" << a.zMin << ", " << a.zMax << ")" << G4endl;
  out << "T: (" << a.tMin << ", " << a.tMax << ")" << G4endl;
  out << static_cast<BDSArray4D const&>(a);
  return out;
}
