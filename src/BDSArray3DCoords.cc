#include "BDSArray3DCoords.hh"

#include <ostream>

#include "globals.hh"

BDSArray3DCoords::BDSArray3DCoords(G4int nXIn, G4int nYIn, G4int nZIn, G4int loopOrder,
				   G4double xMinIn, G4double xMaxIn,
				   G4double yMinIn, G4double yMaxIn,
				   G4double zMinIn, G4double zMaxIn):
  BDSArray4DCoords(nXIn,nYIn,nZIn,1, loopOrder,
		   xMinIn,xMaxIn,
		   yMinIn,yMaxIn,
		   zMinIn,zMaxIn,
		   0,   1)
{;}

std::ostream& operator<< (std::ostream& out, BDSArray3DCoords const &a)
{
  return a.Print(out);
}
