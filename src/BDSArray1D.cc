#include "BDSArray1D.hh"

#include "G4Types.hh"

#include <ostream>

BDSArray1D::BDSArray1D(G4int nX):
  BDSArray2D(nX, 1)
{;}

BDSFieldValue& BDSArray1D::operator()(const G4int x,
				      const G4int /*y*/,
				      const G4int /*z*/,
				      const G4int /*t*/)
{
  return BDSArray4D::operator()(x,0,0,0);
}

const BDSFieldValue& BDSArray1D::operator()(const G4int x,
					    const G4int /*y*/,
					    const G4int /*z*/,
					    const G4int /*t*/) const
{
  return BDSArray4D::operator()(x,0,0,0);
}

std::ostream& operator<< (std::ostream& out, BDSArray1D const &a)
{
  out << "#x = " << a.sizeX() << G4endl;
  
  out << "[ ";
  for (G4int x = 0; x < a.sizeX(); x++)
    {out << a(x,0,0,0) << "\t";}
  out << "]" << G4endl;
  return out;
}

