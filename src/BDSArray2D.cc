#include "BDSArray2D.hh"

#include "G4Types.hh"

#include <ostream>

BDSArray2D::BDSArray2D(G4int nX, G4int nY):
  BDSArray3D(nX, nY, 1)
{;}

BDSFieldValue& BDSArray2D::operator()(const G4int x,
				      const G4int y,
				      const G4int /*z*/,
				      const G4int /*t*/)
{
  return BDSArray3D::operator()(x,y,0,0);
}

const BDSFieldValue& BDSArray2D::operator()(const G4int x,
					    const G4int y,
					    const G4int /*z*/,
					    const G4int /*t*/) const
{
  return BDSArray3D::operator()(x,y,0,0);
}

std::ostream& operator<< (std::ostream& out, BDSArray2D const &a)
{
  out << "#x = " << a.sizeX() << ", ";
  out << "#y = " << a.sizeZ() << G4endl;
  
  for (G4int y = 0; y < a.sizeY(); y++)
    {
      out << "[ ";
      for (G4int x = 0; x < a.sizeX(); x++)
	{out << a(x,y,0,0) << "\t";}
      out << "]" << G4endl;
    }
  out << G4endl;
  return out;
}
