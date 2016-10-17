#include "BDSArray3D.hh"

#include "G4Types.hh"

#include <ostream>

BDSArray3D::BDSArray3D(G4int nX, G4int nY, G4int nZ):
  BDSArray4D(nX,nY,nZ,1)
{;}

std::ostream& operator<< (std::ostream& out, BDSArray3D const &a)
{
  return a.Print(out);
}
