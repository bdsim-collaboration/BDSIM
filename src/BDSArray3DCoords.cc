#include "BDSArray3DCoords.hh"

#include <ostream>

#include "globals.hh"

BDSArray3DCoords::BDSArray3DCoords(G4int nXIn, G4int nYIn, G4int nZIn,
				   G4double xMinIn, G4double xMaxIn,
				   G4double yMinIn, G4double yMaxIn,
				   G4double zMinIn, G4double zMaxIn):
  BDSArray4DCoords(nXIn,nYIn,nZIn,1,
		   xMinIn,xMaxIn,
		   yMinIn,yMaxIn,
		   zMinIn,zMaxIn,
		   0,   1)
{;}

std::ostream& operator<< (std::ostream& out, BDSArray3DCoords const &a)
{
  out << "X: (" << a.xMin << ", " << a.xMax << ")" << G4endl;
  out << "Y: (" << a.yMin << ", " << a.yMax << ")" << G4endl;
  out << "Z: (" << a.zMin << ", " << a.zMax << ")" << G4endl;
  out << static_cast<BDSArray4D const&>(a);
  return out;
}
