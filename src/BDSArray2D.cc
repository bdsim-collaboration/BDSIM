#include "BDSArray2D.hh"

#include "G4Types.hh"

#include <ostream>

BDSArray2D::BDSArray2D(G4int nX, G4int nY):
  BDSArray3D(nX, nY, 1)
{;}

std::ostream& operator<< (std::ostream& out, BDSArray2D const &a)
{
  return a.Print(out);
}
