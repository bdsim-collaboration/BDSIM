#include "BDSArray2D.hh"

#include "G4Types.hh"

#include <ostream>

BDSArray2D::BDSArray2D(G4int nXIn, G4int nYIn):
  BDSArray3D(nXIn, nYIn, 1)
{;}

std::ostream& operator<< (std::ostream& out, BDSArray2D const &a)
{
  return a.Print(out);
}
