#include "BDSArray3D.hh"

#include "G4Types.hh"

#include <ostream>

BDSArray3D::BDSArray3D(G4int nXIn, G4int nYIn, G4int nZIn):
  BDSArray4D(nXIn,nYIn,nZIn,1)
{;}

std::ostream& operator<< (std::ostream& out, BDSArray3D const &a)
{
  return a.Print(out);
}
