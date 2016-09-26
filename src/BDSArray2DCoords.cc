#include "BDSArray2DCoords.hh"

#include <ostream>

#include "globals.hh"

BDSArray2DCoords::BDSArray2DCoords(G4int nXIn, G4int nYIn,
				   G4double xMinIn, G4double xMaxIn,
				   G4double yMinIn, G4double yMaxIn):
  BDSArray3DCoords(nXIn,nYIn,1,
		   xMinIn,xMaxIn,
		   yMinIn,yMaxIn,
		   0,   1)
{;}

std::ostream& operator<< (std::ostream& out, BDSArray2DCoords const &a)
{
  out << "X: (" << a.xMin << ", " << a.xMax << ")" << G4endl;
  out << "Y: (" << a.yMin << ", " << a.yMax << ")" << G4endl;
  out << static_cast<BDSArray4D const&>(a);
  return out;
}
